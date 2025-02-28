#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define PORT 9448
#define SERVER_IP "127.0.0.2"


int main(int argc, char const* argv[])
{
	int client_fd;

	struct sockaddr_in server_address;

	char* hello = "Wiadomość druga wysłana na serwer\n";
	char buffer[1024] = { 0 };

	if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	if( inet_pton(AF_INET, "127.0.0.2", &server_address.sin_addr) == -1)
	{
		perror("Invalid address\n");
		close(client_fd);
		exit(EXIT_FAILURE);
	}

	if( sendto(client_fd, hello, strlen(hello), 0, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		perror("sendto failed");
		close(client_fd);
		exit(EXIT_FAILURE);
	}

	printf("Wiadomość wysłana na serwer: %s\n", hello);

	socklen_t addr_len = sizeof(server_address);
	int n = recvfrom(client_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &addr_len);
	if( n == -1)
	{
		perror("recvfrom failed");
		close(client_fd);
		exit(EXIT_FAILURE);
	}

	buffer[n] = '\0';
	printf("Odpowiedź serwera: %s\n", buffer);

	
	close(client_fd);
	return 0;
}
