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

void recieve(int sock, struct sockaddr* servAddr, socklen_t servLen)
{
	char buffer[BUFFER_SIZE];
	bzero(&buffer, BUFFER_SIZE);
	for (;;)
	{
		if (recvfrom(sock, buffer, BUFFER_SIZE, 0, servAddr, &servLen) < 0)
			dieWithError("recvfrom");
		printf("%s", buffer);
		bzero(&buffer, BUFFER_SIZE);
	}
	close(sock);
	return;
}

int main(int argc, char **argv)
{
	int servSock;
	struct sockaddr_in servAddr;
	const int on = 1;

	if ((servSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket");

	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(3456);

	if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		dieWithError("setsockopt");

	if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
		dieWithError("bind");

	recieve(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr));
	return 0;
}
