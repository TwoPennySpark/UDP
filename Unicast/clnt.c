#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

void dieWithError(char *msg)
{
	printf("[-]ERROR: %s\n", msg);
	exit(0);
}

void chat(int sock, struct sockaddr* servAddr, socklen_t len)
{
	char buffer[BUFFER_SIZE];
	int bufLen;
	socklen_t recvLen;
	struct sockaddr *recvAddr;

	bzero(&recvAddr, sizeof(recvAddr));
	bzero(&buffer, BUFFER_SIZE);
	printf("Enter the message:\n");
	while(fgets(buffer, BUFFER_SIZE, stdin) != NULL)
	{
		bufLen = strlen(buffer);	
		if (sendto(sock, buffer, bufLen, 0, servAddr, len) < 0)
			dieWithError("sendto() failed");
		bzero(&buffer, BUFFER_SIZE);
		
		recvLen = sizeof(recvAddr);
		if (recvfrom(sock, buffer, BUFFER_SIZE, 0, recvAddr, &recvLen) < 0)
			dieWithError("recvfrom() failed");

		printf("Server message:\n%s", buffer);
		bzero(&buffer, BUFFER_SIZE);
		printf("Enter the message:\n");
	}
}
int main(int argc, char ** argv)
{
	int clntSock;
	struct sockaddr_in servAddr;
	
	if (argc != 3)
		dieWithError("usage: <IP address> <port number>");

	if ((clntSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket() failed");

	bzero(&servAddr, sizeof(servAddr));	
	servAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr.s_addr) <= 0)
		dieWithError("inet_pton() failed");
	servAddr.sin_port = htons(atoi(argv[2]));

	chat(clntSock, (struct sockaddr*)&servAddr, sizeof(servAddr));
	close(clntSock);

	return 0;
}
	
