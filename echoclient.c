/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <string.h>

int file_available(int, rio_t, char*);
void save_file(char*, rio_t);

int main(int argc, char **argv){
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = 2121;

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);
    
    // tant que on n'as pas ecrit bye
    int bye = 0;
    while(!bye){ 
        // get command from input
        Fgets(buf, MAXLINE, stdin);

        // exit if bye        
        if(!strcmp(buf, "bye\n")){
            printf("byed\n");
            bye = 1;
            continue;
        }

        // send command to server
        if(!file_available(clientfd, rio, (char*)buf)){
            printf("file doesn't exist: buff: %s.\n", buf);
            continue;
        };

        // change \n to end 0
        int i = 0;
        while (buf[i]!='\n')
            i++;
        buf[i] = 0;

        save_file(buf, rio);
    }

    Close(clientfd);
    exit(0);
}

int file_available(int clientfd, rio_t rio, char* buf){
    // send command to server
    Rio_writen(clientfd, buf, strlen(buf));
    int is_OK;
    Rio_readnb(&rio, &is_OK, 4);
    if(is_OK == 1){
        // fichier existe
        return 1;
    }

    return 0; 

    // a faire
}

void save_file(char* file, rio_t rio){
    // file c'est le nom qu'on a envoye
    
    // get number of max_line
    char file_name[1000] = "./filesClient/";
    
    strcat(file_name, file);
    int fd = open(file_name, O_WRONLY | O_CREAT, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);

    // if server sends OK, then save_file()
    int packet_size;
    Rio_readnb(&rio, &packet_size, 4);
    
    char *packet[MAXLINE];
    int octs_lis = Rio_readnb(&rio, &packet, packet_size);

    while (octs_lis == MAXLINE) {
        Rio_writen(fd, packet, octs_lis);
        Rio_readnb(&rio, &packet_size, 4);
        octs_lis = Rio_readnb(&rio, &packet, packet_size);
    } 
    Rio_writen(fd, packet, octs_lis);
    Close(fd);
}