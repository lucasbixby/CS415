/*
* Description: Project 2 [ helper fucntions ] for MPC v4.0
*
* Author: Lucas Bixby
*
* Date: 05/10/2026 ( last modified )
*/

/*  handle all of the helper functions for:
    -   extracting commands
    -   parsing
    -   signals
    -   memory clean-up
    -   enqueue/dequeue
    -   process-schedular 
    -   displaying process information
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "MCP.h"

int count_lines(FILE *fp) 
// count the lines in an input file 
{
    char *line = NULL;
    size_t len = 0;
    int count = 0;

    while (getline(&line, &len, fp) != -1) {
        count++;
    }

    free(line);

    rewind(fp);

    return count;
}

char **parse_args(char *line, int *count) 
// parse the arguments of a command line
{
    int capacity = 10;

    char **args = malloc(capacity * sizeof(char *));
    if (args == NULL) {
        perror("malloc args");
        *count = 0;
        return NULL;
    }

    *count = 0;

    char *token = strtok(line, " \t\n");

    while (token != NULL) {
        if (*count >= capacity) {
            capacity *= 2;

            char **temp = realloc(args, capacity * sizeof(char *));
            if (temp == NULL) {
                perror("realloc args");
                free(args);
                *count = 0;
                return NULL;
            }

            args = temp;
        }

        args[*count] = token;
        (*count)++;

        token = strtok(NULL, " \t\n");
    }

    char **temp = realloc(args, (*count + 1) * sizeof(char *));
    if (temp == NULL) {
        perror("realloc final args");
        free(args);
        *count = 0;
        return NULL;
    }

    args = temp;
    args[*count] = NULL;

    return args;
}

command_line* extract_commands(char* workload_file, int *num_commands)
// tokenizes an input file of commands seperated by newlines 
// creates an array of commands. all command strings -> individual command arguments 
{
    // 1.) open file 
    // 2.) read each line into a line buffer using getline()
    // 3.) parse each buffer by " " into a command_line struct (command_line.commands) 
    //     and count the number of args (command_line.num_arguments)
    // 4.) store each command_line into the workload array 

    FILE *fp = fopen(workload_file, "r");
    if (fp == NULL) {
        perror("fopen input");
        return NULL; 
    } 

    int count = count_lines(fp);
    *num_commands = count; 
    command_line* workload = malloc(sizeof(command_line) * count);

    char *line = NULL;
    size_t len = 0; 
    ssize_t nread; 
    int i = 0; 

    while ((nread = getline(&line, &len, fp)) != -1) {
        char *line_copy = strdup(line);
        if (line_copy == NULL) {
            perror("strdup");
            continue;
        }

        int argc;
        char **argv = parse_args(line_copy, &argc);

        if (argv == NULL) {
            free(line_copy);
            continue;
        }

        workload[i].command = argv;
        workload[i].num_arguments = argc;

        workload[i].raw_line = line_copy;

        i++;
    }

    free(line);
    fclose(fp);

    *num_commands = i;

    return workload; 
}

void free_workload(command_line *workload, int num_commands)
{
    if (workload == NULL) {
        return;
    }

    for (int i = 0; i < num_commands; i++) {
        free(workload[i].raw_line);
        free(workload[i].command);
    }

    free(workload);
}

void setup_signals(sigset_t *set)
{
    sigemptyset(set);
    sigaddset(set, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, set, NULL) < 0) {
        perror("sigprocmask");
        exit(1);
    }
}

void wait_for_signal(sigset_t *set)
// listen for signals before executing 
{
    int sig;

    if (sigwait(set, &sig) != 0) {
        perror("sigwait");
        exit(1);
    }
}

void enqueue(pid_t *pids, int active, pid_t id)
{   
    pids[active] = id; 
}

pid_t dequeue(pid_t *pids, int active)
{
    pid_t removed = pids[0];
    for ( int i=0; i < active - 1; i++) {
        pids[i] = pids[i+1];
    }
    pids[active - 1] = -1;

    return removed; 
}

void alarm_handler(int sig){}

void process_schedular(pid_t *pids, int launched, int mode)
// process schedular logic
{
    // takes in the array of process ids and handles each one in queue order untill none remain

    int active = launched; 

    while ( active > 0 ) {
        pid_t current = dequeue(pids, active);

        kill(current, SIGCONT);
        alarm(1);
        pause();

        kill(current, SIGSTOP);

        int status;
        pid_t result = waitpid(current, &status, WNOHANG);

        if ( result == 0 ) {
            // process still active -> put back into the queue 
            enqueue(pids, active -1, current);
        }
        else if ( result == current ) {
            // process finished 
            active--; 
        } else {
            perror("waitpid");
            active--;
        }

        // when running part4 "mode" is 4, the table will only display when executing part4
        if (mode == 4){
            display_cycle_data(pids, active, active > 0 ? pids[0] : -1);
        }
    }
}

long get_cpu_ms(pid_t pid)
// return the cpu usage data for a process
{
    char path[256];
    char comm[256];
    char state;
    unsigned long utime = 0;
    unsigned long stime = 0;
    long ticks = sysconf(_SC_CLK_TCK);

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    fscanf(fp,
       "%*d %255s %c "
       "%*s %*s %*s %*s %*s "
       "%*s %*s %*s %*s "
       "%lu %lu",
       comm,
       &state,
       &utime,
       &stime);

    fclose(fp);

    return ((utime + stime) * 1000) / ticks;
}

void display_cycle_data(pid_t *pids, int active, pid_t next_pid)
// display the process information for each cycle 
{
    printf("\n");
    printf("-----------------------------------------------------------------\n");
    printf("%-10s %-16s %-18s %-10s %-10s\n",
           "PID", "CMD", "STATE", "CPU(ms)", "MEM(KB)");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < active; i++) {
        pid_t pid = pids[i];

        char path[256];
        char line[256];
        char command[128] = "unknown";
        char state[64] = "?";
        long memory_kb = 0;
        long cpu_ms = get_cpu_ms(pid);

        // read the command name
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        FILE *comm_fp = fopen(path, "r");

        if (comm_fp != NULL) {
            if (fgets(command, sizeof(command), comm_fp) != NULL) {
                command[strcspn(command, "\n")] = '\0';
            }
            fclose(comm_fp);
        }

        // then read the state and memory usage
        snprintf(path, sizeof(path), "/proc/%d/status", pid);
        FILE *status_fp = fopen(path, "r");

        if (status_fp != NULL) {
            while (fgets(line, sizeof(line), status_fp) != NULL) {
                if (strncmp(line, "State:", 6) == 0) {
                    sscanf(line, "State: %63[^\n]", state);
                }

                if (strncmp(line, "VmRSS:", 6) == 0) {
                    sscanf(line, "VmRSS: %ld", &memory_kb);
                }
            }

            fclose(status_fp);
        }

        // print the formatted data 
        printf("%-10d %-16s %-18s %-10ld %-10ld\n",
               pid,
               command,
               state,
               cpu_ms,
               memory_kb);
    }

    printf("-----------------------------------------------------------------\n");
    printf("Current scheduler decision:\n");

    if (next_pid > 0) {
        printf("-> Next process: %d\n", next_pid);
    } else {
        printf("-> Next process: none\n");
    }

    printf("-----------------------------------------------------------------\n");
    fflush(stdout);
}
