#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/wait.h>  


void fork_bomb(int depth)
{
    	pid_t pid1 = fork();

    	if(pid1 < 0)
    	{
        	perror("Error creating new process");
        	printf("Program terminated after: %d proccesses.\n\n", depth-1);
		return;
    	}
    	else if(pid1 == 0)
    	{
        	printf("Running child process number: %d, PID: %d, PPID: %d\n", depth, getpid(), getppid());
        	fork_bomb(depth + 1);
		sleep(2);
        	printf("Process number: %d ended, PID: %d\n", depth, getpid());
        	exit(0);
    	}
    	else
    	{
        	wait(NULL);
        	printf("Parent process number: %d ended, PID: %d\n", depth - 1, getpid());
    	}
}

int main()
{
	printf("Chain of processes started.Parent proccess PID:  %d\n",getpid());
    	fork_bomb(1);
    	printf("Chain of processes ended.\n");
    	return 0;
}
