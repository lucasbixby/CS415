#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void signaler(pid_t* pid_ary, int size, int signal);

int main(int argc, char* argv[])
{
	

	if (argc < 2){

		printf("Input: ./main <num_processes>\n");
		exit(-1);

	}

	// initialization of pid like lab 4

	int size = atoi(argv[1]);

	pid_t *pid_ary = (pid_t *)malloc(sizeof(pid_t) * size);


	// initialize sigset
	sigset_t sigset;
	int sig;

	// create an empty sigset_t
	sigemptyset(&sigset);

	// use sigaddset() to add the SIGUSR1 signal to the set
	sigaddset(&sigset, SIGUSR1);

	// use sigprocmask() to add the signal set in the sigset for blocking
	if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) { 
        perror("sigprocmask");
        exit(1);
    }

	
	for(int i = 0; i < size; i++)
	{

		pid_ary[i] = fork();

		if(pid_ary[i] == 0)
		{
			// print: Child Process: <pid> - Waiting for SIGUSR1…
			printf("Child Process: <%d> - Waiting for SIGUSR1…\n", getpid());

			// wait for the signal
			if (sigwait(&sigset, &sig) != 0) {
				perror("sigwait");
				exit(1);
			}
			
			// print: Child Process: <pid> - Received signal: SIGUSR1 - Calling exec().
			printf("Child Process: <%d> - Received signal: SIGUSR1 - Calling exec().\n", getpid());

			// call execvp with ./iobound like in lab 4
			char *args[] = {"./iobound", "-seconds", "10", NULL};
			execvp(args[0], args);

			perror("Execvp");
			exit(1);

		}
		else if (pid_ary[i] < 0) {
			// fork failed error
			exit(-1);
		}
	}
	
	// send SIGUSR1 
	signaler(pid_ary, size, SIGUSR1);

	// send SIGSTOP 
	signaler(pid_ary, size, SIGSTOP);

	// send SIGCONT
	signaler(pid_ary, size, SIGCONT);

	// send SIGINT
	signaler(pid_ary, size, SIGINT); 



	free(pid_ary);
	
	return 0;
}

void signaler(pid_t* pid_ary, int size, int signal)
{
	// sleep for three seconds
	sleep(3);

	for(int i = 0; i < size; i++)
	{
		// print: Parent process: <pid> - Sending signal: <signal> to child process: <pid>
		printf("Parent process: <%d> - Sending signal: <%d> to child process: <%d>\n", getpid(), signal, pid_ary[i]);

		// send the signal
		kill(pid_ary[i], signal);


	}
}
