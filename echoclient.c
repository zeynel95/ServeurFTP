/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE], res[MAXLINE];
    rio_t rio;
    int n;

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

        Fgets(buf, MAXLINE, stdin);
        Rio_writen(clientfd, buf, strlen(buf));
        int i = 0;
        printf("%s", res);
        while (buf[i]!='\n'){
            fprintf(stderr,"s");
            i++;}        
            printf("s");
        buf[i] = 0;
        fprintf(stderr, "\n%s\n", buf);
        int fd = open(buf, O_WRONLY | O_CREAT, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);
        perror("");
        fprintf(stderr,"a");
        while ((n = Rio_readnb(&rio, res, MAXLINE)) > 0) {
            // Fputs(buf, stdout);
            fprintf(stderr,"n = %d\n", n);
            printf("fd : %d\n", fd);
            Rio_writen(fd, res, n);
        }
            fprintf(stderr,"n = %d\n", n);
    Close(clientfd);
    Close(fd);
    exit(0);
}
