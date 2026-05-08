#ifndef MCP_H
#define MCP_H

#include <stdio.h>
#include <signal.h>

typedef struct
{
    char** command;         /* Array of individual arguments separated by ' ' */
    int num_arguments;      /* Number of arguments found in a single command */
    char *raw_line; 
} command_line;

int count_lines(FILE *fp);

char **parse_args(char *line, int *count);

command_line* extract_commands(char* workload_file, int *num_commands);

void free_workload(command_line *workload, int num_commands);

void setup_signals(sigset_t *set);

void wait_for_signal(sigset_t *set);

void process_schedular(pid_t *pids, int launched);

void alarm_handler(int sig);

#endif
