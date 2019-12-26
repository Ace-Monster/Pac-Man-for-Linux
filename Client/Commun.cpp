#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include "Commun.h"

bool Commun::createConnect() {
	struct hostent *host = gethostbyname(chost);
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_sock;
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(post);
	server_sock.sin_addr = *((struct in_addr *)host->h_addr);
	if (connect(sockfd, (struct sockaddr *)&server_sock, sizeof(struct sockaddr)) == -1) {
		perror("Connect error\n");
		return false;
	}
	return true;
}