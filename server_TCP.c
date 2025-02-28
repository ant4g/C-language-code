#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define PORT 9448

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address, client_addr;
    socklen_t addrlen = sizeof(address);
    
    char buffer[1024] = { 0 };

   
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket failed\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("Bind failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 2) < 0)
    {
        perror("Listen failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Serwer nasłuchuje na porcie %d\n", PORT);

    for (int i = 0; i < 2; i++) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen)) < 0)
        {
            perror("Accept failed\n");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd); 
            read(new_socket, buffer, 1024);
            printf("Klient %d: %s\n", i + 1, buffer);
            
	    char client_ip[INET_ADDRSTRLEN];
	    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
	    char res[1024];
	    snprintf(res, sizeof(res), "(Adres klienta: %s)", client_ip);
	    strcat(buffer, res );
	    send(new_socket, buffer, strlen(buffer), 0);
            printf("Wiadomość wysłana do klienta %d\n", i + 1);

            close(new_socket);
            exit(0); 
 	} else if (pid < 0) {
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
        }

        close(new_socket);
    }

    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    printf("Serwer zakończył działanie\n");
    return 0;
}

