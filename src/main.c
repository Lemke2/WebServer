#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "8080"
#define BACKLOG 10

// A basic mental model I have developed for how network programming works is the following:

// 0) Decide which type of webserver we want(TCP/UDP)
// 1) Use that decision to create addrinfo to resolve your hostname
// 2) Create the socket using the socket call using addrinfo
// 3) Bind the socket to the hostname we got using addrinfo
// 4) Listen on the socket
// 5) Handle requests

// This might be wrong - but from the first 6 chapters of Beej's guide this is what I can gather


void* get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)-> sin_addr);
    }
    else{
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}
int main(){
    // Step 1 from the ReadMe -> resolving the hostname
    struct addrinfo hints;
    struct addrinfo* serverinfo;
    struct addrinfo* current;
    int errorValue = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    errorValue = getaddrinfo(NULL, PORT, &hints, &serverinfo);
    if(errorValue != 0){
        printf("getaddrinfo:: %s\n", gai_strerror(errorValue));
        return 1;
    }

    printf("The server info is in IPV mode: %d\n", serverinfo->ai_family); // 2 is IPV4 and 10 is IVP6
    printf("The server info is in TCP/UDP mode: %d\n", serverinfo->ai_socktype); // 1 is TCP and 2 is UDP

    
    // STEP 2 and 3 -> Creating a socket and binding
    int sockfd;
    int yes = 1; // setsockopt need an address to a boolean instead of the boolean itself
    for(current = serverinfo; current != NULL; current = current->ai_next){
        sockfd = socket(current->ai_family, current->ai_socktype, current->ai_protocol);    
        if(sockfd == -1){
            printf("server: Socket construction error. Going to next element of the hostname list\n");
            continue;
        }

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            printf("Error setting socket options. Going to next element of the hostname list\n");
            return 1;
        }

        if(bind(sockfd, current->ai_addr, current->ai_addrlen) == -1){
            close(sockfd);
            printf("Bind failed. Going to next element of the hostname list\n");
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    // STEP 4 -> Listen
    if(listen(sockfd, BACKLOG) == -1){
        printf("Listen error.\n");
        return 1;
    }

    // STEP 5 -> Handle Requests
    
    int newfd;
    socklen_t requestAddressSize;
    struct sockaddr_storage requestAddress;
    char humanReadableIPAddress[INET6_ADDRSTRLEN];
    while(1){
        requestAddressSize = sizeof(requestAddress);
        newfd = accept(sockfd, (struct sockaddr*)&requestAddress, &requestAddressSize);
        if (newfd == -1) {
            printf("Accept error.\n");
            continue;
        }

        inet_ntop(requestAddress.ss_family, get_in_addr((struct sockaddr *)&requestAddress), humanReadableIPAddress, sizeof humanReadableIPAddress);
        printf("client: connected to %s\n", humanReadableIPAddress);

        if(fork() == 0){
            close(sockfd);
            if(send(newfd, "HELLO WORLD!", 12, 0) == -1){
                printf("Send error.\n");
            }
            close(newfd);
            exit(0);
        }
        close(newfd);
    }


    return 0;
}