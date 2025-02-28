#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 9448

int main(int arg, char const* argv[])
{
	int server_fd;

	struct sockaddr_in address, client_addr;
	char buffer[1024] = { 0 };
	socklen_t addrlen = sizeof(client_addr);

	if(( server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket operation failed\n");
		exit(EXIT_FAILURE);
	}

	memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if( bind( server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		perror("Bind failed\n");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Serwer działa\n");

	int n = recvfrom( server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addrlen);
	if( n == -1)
	{
		perror("recvfrom failed\n");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	buffer[n] = '\0';
	printf("Odebrano wiadomość: %s\n", buffer);
	

	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
	char res[1024];
	snprintf(res, sizeof(res), "(Adres klienta: %s)", client_ip);
	
	strcat(buffer, res);

	if( sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, addrlen ) == -1)
	{
		perror("sendto failed\n");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	printf("Odesłano wiadomość do klienta\n");

	close(server_fd);

	return 0;
}
