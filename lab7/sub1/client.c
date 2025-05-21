#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5555
#define ADDRESS "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ADDRESS);
    server_addr.sin_port = htons(PORT);

    fgets(message, BUFFER_SIZE - 1, stdin);

    while(message[0] != '\0') {
        sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
	
	int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);

	if (n > 0) {
	    buffer[n] = '\0';
	    printf(buffer);
	} else {
	    perror("recvfrom failed");
	}

	fgets(message, BUFFER_SIZE - 1, stdin);
    }

    close(sockfd);
    return 0;
}
