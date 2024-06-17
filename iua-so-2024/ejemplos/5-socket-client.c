#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
void func(int sockfd)
{
    char buff[MAX];
    int n;
    while ( 1 ) {
        bzero(buff, sizeof(buff));                       // clean buff
        printf("C: ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n');         // read from stdin
        // 3
        write(sockfd, buff, sizeof(buff));               // write to server
        bzero(buff, sizeof(buff));                       // clean
        // 4
        read(sockfd, buff, sizeof(buff));
        printf("S: %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main( int argc, char *argv[] )
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // 1
    sockfd = socket(AF_INET, SOCK_STREAM, 0);                   // create socket

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                              // set addr
    servaddr.sin_addr.s_addr = inet_addr( argv[1] );
    servaddr.sin_port = htons(PORT);

    // 2
    connect(sockfd, (SA*)&servaddr, sizeof(servaddr));          // connect

    func(sockfd);                                               // do chat write read
    // 5
    close(sockfd);                                              // close
}

