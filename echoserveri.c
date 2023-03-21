/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256

void echo(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    int dad = getpid();

    for(int i = 0; i < 5 ; i++){
        Fork();
    }
    // idee pour le pool tab de process qui exec le Accept
    while (1) {
        if (getpid() != dad)
        {
            // connfd < 0 = code erreur
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            if (connfd >= 0)
            {
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                        INET_ADDRSTRLEN);
                
                printf("server connected to %s (%s)\n", client_hostname,
                    client_ip_string);

                echo(connfd);
                Close(connfd);
                // exit(0);
            }
            else Close(connfd);
        }
        
    }
    while(waitpid(-1, NULL, 0)>0);
    exit(0);
}

