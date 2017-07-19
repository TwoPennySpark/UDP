#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define BUFFER_SIZE 128

void dieWithError(char *msg)
{
	printf("[-]ERROR: %s\n", msg);
	exit(0);
}

void chat(int servSock, struct sockaddr* servAddr, socklen_t len)
{
	char buffer[BUFFER_SIZE];
	size_t bufLen;

	bzero(&buffer, BUFFER_SIZE);
	for(;;)
	{
		if (recvfrom(servSock, &buffer, BUFFER_SIZE, 0, servAddr, &len) < 0)
			dieWithError("recvfrom() failed");
		printf("Client message:\n%s", buffer);
		bzero(&buffer, BUFFER_SIZE);

		printf("Enter the message:\n");
		if (fgets(buffer, BUFFER_SIZE, stdin) < 0)
			dieWithError("fgets() failed");
		bufLen = strlen(buffer);
		if (sendto(servSock, &buffer, bufLen, 0, servAddr, len) < 0)
			dieWithError("sendto() failed");	
		
		bzero(&buffer, BUFFER_SIZE);
	} 	
} 

int main(int argc, char ** argv)
{
	int servSock;
	struct sockaddr_in servAddr;

	if (argc != 2)
		dieWithError("usage: <port number>");

	if ((servSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket() failed");

	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(atoi(argv[1])); 

	if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
		dieWithError("bind() failed");

	chat(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr));

	return 0;
}	
