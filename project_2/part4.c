/*
* Description: Project 2 [ part 4 ] for MPC v4.0
*
* Author: Lucas Bixby
*
* Date: 05/10/2026 ( last modified )
*/

/*
    DEV NOTE:

        Pick up here, next we will need to provide information on each child 
        process using /proc, extract relevent information from the table and 
        display it in a formatted table in the console. 
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

void handle_workload(command_line* workload, int num_commands, sigset_t *set)
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
            // child waits until parent sends SIGUSR1
            wait_for_signal(set);

            // child becomes workload program 
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

    // send SIGUSR1 to each child so it can continue to execvp()
    for (int i = 0; i < launched; i++) { 
        kill(pids[i], SIGUSR1);
    }

    sleep(1);

    for (int i = 0; i < launched; i++) {
        kill(pids[i], SIGSTOP);
    }

    // schedular logic 
    signal(SIGALRM, alarm_handler);
    process_schedular(pids, launched, 4);

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

        sigset_t set;
        setup_signals(&set);

        handle_workload(workload, num_commands, &set);

        free_workload(workload, num_commands);

    } else {
        // there is an invalid number of arguments -> show usage
        fprintf(stderr, "Usage: ./MPC [-f workload]\n");
        return -1; 
    }

    return 0; 
}
