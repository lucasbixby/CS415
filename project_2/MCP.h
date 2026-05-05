#ifndef MCP_H
#define MCP_H

typedef struct
{
    char** command;         /* Array of individual arguments separated by ' ' */
    int num_arguments;      /* Number of arguments found in a single command */
    char *raw_line; 
} command_line;

command_line* extract_commands(char *workload_file, int *num_commands);

#endif