#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>


#define KEY 0x1234

int main()
{
	int shm_id;
	int *shared_counter;

	shm_id = shmget(KEY, sizeof(int), IPC_CREAT | 0666);
	if( shm_id == -1)
	{
		perror("Shmget failed");
		exit(EXIT_FAILURE);
		
	}

	shared_counter =(int *)shmat(shm_id, NULL, 0);
	if( shared_counter == (int *) -1)
	{
		perror("Attaching failed");
		shmctl(shm_id, IPC_RMID, NULL);
		exit(EXIT_FAILURE);
	}
	
	*shared_counter = 0;
	printf("Wartość aktualna wspólnej zmiennej %d\n", *shared_counter);
	
	for( int i =0 ; i < 5; i++)
	{
		pid_t pid;
		pid = fork();
		
		if( pid < 0)
		{
			perror("Fork failed");
			shmdt(shared_counter);
			shmctl(shm_id, IPC_RMID, NULL);
			exit(EXIT_FAILURE);
		}
		else if( pid == 0)
		{
			pid_t child_pid = getpid();
			for( int j = 0; j < 1; j++)
			{

				int local = 0;
				local = *shared_counter;
				printf("Process %d, read variable: %d\n", child_pid, local);

				local += 1;

				printf("Process %d, increased variable to: %d\n", child_pid, local);
				printf("Process %d, is sleeping for 1 sec\n", child_pid);
				sleep(1);

				*shared_counter = local;

				printf("Process: %d, add value to: %d\n", child_pid, *shared_counter);
			}
		
			printf("Child process %d, ended\n", child_pid);
			shmdt(shared_counter);
			exit(EXIT_SUCCESS);
		}
	}
	for( int i =0; i < 5; i++)
	{
		wait(NULL);
	}

	printf("Final value of couter is: %d\n", *shared_counter);
	shmdt(shared_counter);
	shmctl(shm_id, IPC_RMID, NULL);
	exit(EXIT_FAILURE);

	return 0;
}

