/*
* Description: Project 2 [ helper fucntions ] for MPC v2.0
*
* Author: Lucas Bixby
*
* Date: 05/07/2026 ( last modified )
*/

/*  handle all of the helper functions for:
    -   extracting commands
    -   parsing
    -   signals
    -   memory clean-up
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
