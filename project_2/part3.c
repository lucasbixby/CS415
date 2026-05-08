/*
* Description: Project 2 [ part 3 ] for MPC v3.0
*
* Author: Lucas Bixby
*
* Date: 05/08/2026 ( last modified )
*/

/*
    DEV NOTE:

        in part 3 we now need to implement a schedular that allows each program to run for 1 
        seccond, then stop to choose the next program to execute then start that one. We need 
        to use the alarm(2) system call to set a timer for each child process. Addtionally the 
        schedular waits for the SIGALRM before choosing the next process to execute. 
        
        strategy:

            because we now have a way to stop and continue child processes, we can implement that within 
            another helper function to : start process -> stop after 1 time interval -> choose next 
            process to execute -> start process. 

        How will we implement this?

            what if we create a helper function called process_schedular() that opperates on a 
            queue of child processes. Every time we fork() in the handle_workload() function 
            we add the child process to the queue in the process_schedular() handler. Inside the
            process_schedular() we will have a while loop that will loop over the waiting process
            queue and execute each CP ( child process ) for the one second before stoping it ->
            dequing it -> and enqueueing it, if it is still not finnished. the while loop continues 
            untill there are no more CPs in the process queue. Therefore we can implement the 
            process scheduling using a round-robbin technique. 

        implementation flow:

            fork all children
            children wait for SIGUSR1
            parent sends SIGUSR1 so children exec
            parent immediately stops all children
            choose first process
            SIGCONT first process
            alarm(1)

            when SIGALRM happens:
                SIGSTOP current process
                choose next unfinished process
                SIGCONT next process
                alarm(1)
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
    process_schedular(pids, launched);

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
