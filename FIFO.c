#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>

#define FIFO_PATH "/tmp/my_fifo"
int main() {
    int fd; 

    mkfifo(FIFO_PATH, 0666);


    pid_t pid = fork();

    if (pid < 0) {
        perror("Nie udało się utworzyć procesu");
        exit(EXIT_FAILURE);
    }

    if (pid > -1) {
	
	fd = open( FIFO_PATH, O_WRONLY);	
	
	if( fd == -1)
	{
		perror("Error opening FIFO");
		exit(EXIT_FAILURE);
	}
        printf("Rodzic: Rozpoczynam wysyłanie danych...\n");

        char message[1024]; 
        for(int i=0; i< 1024; i++)
	{
		message[i] = 'A';
	}
	int bytes_sent = 0;

        while (1) {
            ssize_t written = write(fd, message, sizeof(message));
            if (written == -1) 
	    {
                    perror("Rodzic: Błąd podczas pisania");
                    break;
            }
            else 
	    {
                bytes_sent += written;
                printf("Rodzic: Wysłano %ld bajtów. Łącznie wysłano %d bajtów.\n", written, bytes_sent);
            }
        }

        close(fd); 
    } else {

        printf("Dziecko: Rozpoczynam odbieranie danych z opóźnieniem...\n");

        char buffer[1024]; 
        int bytes_received = 0;

        
	fd = open(FIFO_PATH, O_RDONLY);

	if( fd == -1)
	{
		perror("Error opening FIFO");
		exit(EXIT_FAILURE);
	}
        
	while (1) {
            sleep(1);
	    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                bytes_received += bytes_read;
                printf("Dziecko: Odebrano %ld bajtów. Łącznie odebrano %d bajtów.\n", bytes_read, bytes_received);
            } else if (bytes_read == 0) {
                printf("Dziecko: Koniec danych w potoku.\n");
                break;
            } else {
                perror("Dziecko: Błąd podczas odczytu z potoku");
                break;
            }
        }

        close(fd); 
    }
    unlink(FIFO_PATH);
    return 0;
}
