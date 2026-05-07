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
            // replace the child with the request command 
            execvp(workload[i].command[0], workload[i].command);

            perror("Execvp");
            free(pids);
            free_workload(workload, num_commands);
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
            fprintf(stderr, "Usage: ./part1 -f <workload>\n");
            return -1; 
        }
        // extract the commands from the workload file here:
        int num_commands;
        command_line* workload = extract_commands(argv[2], &num_commands);
        handle_workload(workload, num_commands);

        free_workload(workload, num_commands);

    } else {
        // there is an invalid number of arguments -> show usage
        fprintf(stderr, "Invalid use: incorrect number of parameters\n");
        return -1; 
    }

    return 0; 
}
