/*
* Description: Project 2 [ part 2 ] for MPC v2.0
*
* Author: Lucas Bixby
*
* Date: 05/06/2026 ( last modified )
*/

// 1.)  implement a way for the MCP to stop all forked (MCC) child processes right before they call exec().

// 2.)  implement the needed mechanism for the MCP to signal a running process to stop
//      (using the SIGSTOPsignal) and then to continue it again (using the SIGCONTsignal). 

/*
DEV NOTE:
    finished implementing forked processes to wait for SIGUSR1 to execute. Next work on step 2 
    where you will need to allow the MCP to stop a running process, along with letting it 
    continue a process, using SIGSTOP and SIGCONT . 
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
            // in child process:
            wait_for_signal(set);

            // replace the child with the request command 
            execvp(workload[i].command[0], workload[i].command);

            perror(workload[i].command[0]);
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

    for (int i = 0; i < launched; i++) { 
        waitpid(pids[i], NULL, 0);
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

        sigset_t set;
        setup_signals(&set);

        handle_workload(workload, num_commands, &set);

    } else {
        // there is an invalid number of arguments -> show usage
        fprintf(stderr, "Usage: ./MPC [-f workload]\n");
        return -1; 
    }

    return 0; 
}
