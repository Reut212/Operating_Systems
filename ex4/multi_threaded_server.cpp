/*
** server.c -- a stream socket server demo
*/

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
#include <pthread.h>
# include "funcs.hpp"
pthread_mutex_t lock;


#define MAXDATASIZE 100 // max number of bytes we can get at once

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 60   // how many pending connections queue will hold

void *socketThread(void *arg) {
    int self = pthread_self();
    int newSocket = *((int *) arg);
    printf("------------------\n");
    printf("client %d connected\n", self);
    char buf[6];
    while (1) {
        memset(buf, 0, 6);
        if ((recv(newSocket, buf, 6, 0)) == -1)
            perror("recv");
        if (strcmp(buf, "PUSH") == 0) {
            char data_push[1024];
            if ((recv(newSocket, data_push, 1024, 0)) == -1)
                perror("recv");
            else{
                pthread_mutex_lock(&lock);
                PUSH(data_push);
                printf("client %d pushed %s to the stack\n", self, data_push);
                pthread_mutex_unlock(&lock);
            }
        } else if (strcmp(buf, "POP") == 0) {
            pthread_mutex_lock(&lock);
            POP();
            printf("popped from stack\n");
            pthread_mutex_unlock(&lock);

        } else if (strcmp(buf, "TOP") == 0) {
            pthread_mutex_lock(&lock);
            char* top = TOP();
            if (send(newSocket, top, 1024, 0) == -1)
                perror("send");
            pthread_mutex_unlock(&lock);
        }
        else if (strcmp(buf, "EXIT") == 0) {
            printf("client %d disconnected\n", self);
            close(newSocket);
            pthread_exit(NULL);
        }
    }
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(void) {

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    pthread_t tid[60];
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    int i = 0;
    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if (pthread_create(&tid[++i], NULL, socketThread, &new_fd) != 0)
            printf("Failed to create thread\n");
//
//        pthread_join(tid[i],NULL);
//        i++;

    }
    pthread_mutex_destroy(&lock);
    return 0;
}