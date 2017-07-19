#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256

void dieWithError(char *msg)
{
	printf("[-]ERROR: %s() failed", msg);
	exit(0);
}

void broadcast(int sock, struct sockaddr *servAddr, socklen_t servLen)
{
	char sendline[BUFFER_SIZE];
	const int on = 1;
	int numbytes;

	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
		dieWithError("setsockopt");
	printf("Print the message:\n");
	while (fgets(sendline, BUFFER_SIZE, stdin) != NULL)
	{
		numbytes = strlen(sendline);
		if ((sendto(sock, sendline, numbytes, 0, servAddr, servLen)) < 0)
			dieWithError("sendto");
		bzero(&sendline, strlen(sendline));
	}
	close(sock);
	return;
}

int main(int argc, char **argv)
{
	int clntSock;
	struct sockaddr_in servAddr;

	if (argc != 2)
	{
		printf("Usage: <broadcast address>\n");
		exit(0);
	}

	if ((clntSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket");

	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr.s_addr) < 0)
		dieWithError("inet_pton");
	servAddr.sin_port = htons(3456);

	broadcast(clntSock, (struct sockaddr*)&servAddr, sizeof(servAddr));

	return 0;
}