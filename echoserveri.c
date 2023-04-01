/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256

#ifndef NB_PROC
#define NB_PROC 5
#endif

int Tpid[NB_PROC];

void ajouter(int p){
	int  i = 0;
	while (Tpid[i] != -1)
	{
		i++;
	}
	Tpid[i] = p;
}

void init(){
	for (int i = 0; i < NB_PROC; i++)
	{
		Tpid[i] = -1;
	}
}

void echo(int connfd);

void handlerQUIT(int sig){
    int pid;
    for (int i = 0; i < NB_PROC; i++)
    {
        if(Tpid[i] != -1){
            pid = Tpid[i];
            Kill(pid, SIGINT);
            Tpid[i] = -1;
            waitpid(pid, NULL, WNOHANG);
        }
    }
    return;
}


/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv){
    Signal(SIGQUIT, handlerQUIT);
    
    /*on initialise le tableau du pool de processus*/
    init();

    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(0);
    }
    port = 2121;
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    int dad = getpid();
    int pid;
    for(int i = 0; i < NB_PROC ; i++){
        if(getpid() == dad){
            pid = Fork();
            if(pid != 0)
                ajouter(pid);
        }
    }
    // idee pour le pool tab de process qui exec le Accept
    while (1) {
        if (getpid() != dad){
            // connfd < 0 = code erreur
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            if (connfd >= 0){
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                        INET_ADDRSTRLEN);
                
                printf("server connected to %s (%s)\n", client_hostname,
                    client_ip_string);
                echo(connfd);
                printf("server deconnected to %s (%s)\n", client_hostname, client_ip_string);


                Close(connfd);
            }
            else Close(connfd);
        }
        
    }
    while(waitpid(-1, NULL, 0)>0);
    exit(0);
}

