/*
* Description: Project 2 [ part 1 ] for MPC v1.0
*
* Author: Lucas Bixby
*
* Date: 05/05/2026 ( last modified )
*/

// 1.) read the program workload from the specified input file 

// 2.) For each command, your MCP must launch a seprate process to runn the command
//     using some variant of the following syscalls 
//          - fork()
//          - exec()

// 3.) Once all of the processes are running, your MCP must wait for each process
//     to terminate using one of the wait family syscalls ( wait() )

// 4.) After all processes have terminated, your MCP must use the exit() syscall

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void handle_workload(command_line* workload, int num_commands)
// handle the workload for the command input file by running parallel processes
{
    // create an array of PIDs for the sucessful forked processes 
    pid_t *pids = malloc(num_commands * sizeof(pid_t));
    if (pids == NULL) {
        perror("malloc");
        return;
    }

    int launched = 0;

    for( int i = 0; i < num_commands; i++ ){
        if (workload[i].num_arguments == 0) {
            continue; 
        }

        // create a new child process
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // in child process:

            // replace the child with the request command 
            execvp(workload[i].command[0], workload[i].command);

            perror(workload[i].command[0]);
            exit(1);
        }

        // store PID so we can wait for it later 
        pids[launched] = pid;
        launched++;
    }

    // wait for all launched child processes
    for (int i = 0; i < launched; i++) { 
        int status;
        waitpid(pids[i], &status, 0);
    }

    free(pids);
}

int main(int argc, char* argv[])
{
    // validate that the program is executed properly ( file mode ) [ ./MPC -f workload ]

    // first we must check what mode our program has started in 
    if ( argc == 3 ) {
        if ( strcmp(argv[1], "-f") != 0 ){
            fprintf(stderr, "Usage: ./MPC -f <workload>\n");
            return -1; 
        }
        // extract the commands from the workload file here:
        int num_commands;
        command_line* workload = extract_commands(argv[2], &num_commands);
        handle_workload(workload, num_commands);

    } else {
        // there is an invalid number of arguments -> show usage
        fprintf(stderr, "Usage: ./MPC [-f workload]\n");
        return -1; 
    }

    return 0; 
}