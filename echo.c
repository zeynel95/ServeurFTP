/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"



void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    fprintf(stderr,"hi");
    n = Rio_readlineb(&rio, buf, MAXLINE);
    fprintf(stderr,"hi");
    buf[n-1] = 0;
    fprintf(stderr, "%s", buf);
    printf("server received %u bytes\n", (unsigned int)n);
    int fd = open(buf, O_RDONLY);
    rio_t fileRio;
    rio_readinitb(&fileRio, fd);

    while((n = Rio_readnb(&fileRio, buf, MAXLINE)) > 0){
        Rio_writen(connfd, buf, n);
    }
}