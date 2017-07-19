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
	int recvSock;
	const int on = 1;
	socklen_t servLen;
	struct sockaddr_in multicAddr, sendAddr;
	struct ip_mreq mreq;
	char buffer[BUFFER_SIZE];
	char addr[BUFFER_SIZE];

	if (argc != 3)
		dieWithError("Usage: <adress of multicast group> <port>");

	if ((recvSock =  socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		dieWithError("socket failed");

	bzero(&multicAddr, sizeof(multicAddr));
	multicAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &multicAddr.sin_addr.s_addr) < 0)
		dieWithError("inet_pton");
	multicAddr.sin_port = htons(atoi(argv[2]));

	if (setsockopt(recvSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		dieWithError("setsockopt1 failed");
	if (bind(recvSock, (struct sockaddr*)&multicAddr, sizeof(multicAddr)) < 0)
		dieWithError("bind failed");

	if (inet_pton(AF_INET, argv[1], &mreq.imr_multiaddr.s_addr) < 0)
		dieWithError("inet_pton failed");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(recvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		dieWithError("setsockopt2 failed");

	servLen = sizeof(sendAddr);
	for(;;)
	{
		recvfrom(recvSock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sendAddr, &servLen);
		printf("Message from IP:%s port:%d:\n%s",
											inet_ntop(AF_INET, &sendAddr.sin_addr.s_addr, addr, BUFFER_SIZE),
											ntohs(sendAddr.sin_port), buffer);
		bzero(&buffer, sizeof(buffer));
	}

	return 0;
}
