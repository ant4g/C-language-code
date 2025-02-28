#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define PORT 9448

int main(int argc, char const* argv[])
{
    int client_fd;
    int status, valread;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket failed\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; i++) {
        send(client_fd, hello, strlen(hello), 0);
        printf("Wiadomość wysłana: %s\n", hello);

        valread = read(client_fd, buffer, 1024);
        buffer[valread] = '\0'; 
        printf("Odpowiedź od serwera: %s\n", buffer);

        sleep(2);
    }

    close(client_fd);
    printf("Klient zakończył działanie\n");
    return 0;
}
