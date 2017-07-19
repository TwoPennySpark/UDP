#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 256

void dieWithError(char *msg)
{
	perror(msg);
	exit(-1);
}


int main(int argc, char **argv)
{
	int clntSock;
	struct sockaddr_in servAddr;
	char buffer[BUFFER_SIZE];
	const int on = 1;

	if (argc != 3)
		dieWithError("Usage: <multicast address> <port>\n");

	if ((clntSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket");
	if (setsockopt(clntSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		dieWithError("setsockopt1 failed");
	
	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr.s_addr) < 0)
		dieWithError("inet_pton");
	servAddr.sin_port = htons(atoi(argv[2]));

	printf("Enter the message:\n");
	while (fgets(buffer, BUFFER_SIZE, stdin)!= NULL)
	{
		printf("%s\n", buffer);
		sendto(clntSock, &buffer, strlen(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
		bzero(&buffer, BUFFER_SIZE);
	}
	return 0;
}
