#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void func(int connfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    while ( 1 ) { 
        bzero(buff, MAX);                               // clean buff
        // 5
        read(connfd, buff, sizeof(buff));               // read from client
        printf("C: %sS: ", buff);
        bzero(buff, MAX);                               // clean
        n = 0; 
        while ((buff[n++] = getchar()) != '\n');        // read msg from stdin 
        // 6
        write(connfd, buff, sizeof(buff));              // write to client
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 

// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
    // 1
    sockfd = socket(AF_INET, SOCK_STREAM, 0);           // create socket
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;                      // configure addr
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
    // 2
    bind(sockfd, (SA*)&servaddr, sizeof(servaddr));     // bind
    // 3
    listen(sockfd, 5);                                  // listen
    len = sizeof(cli); 
    // 4
    connfd = accept(sockfd, (SA*)&cli, &len);           // accept

    func(connfd);                                       // do chat read() write()
    // 7
    close(sockfd); 
}

