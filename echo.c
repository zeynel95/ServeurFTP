/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"


void lis_fic(char* nom, char* res, size_t* n){
    int i = 0;
    char temp;
    while(nom[i] != 0 && nom[i] != '\n')
        i++;
    nom[i] = 0;
    FILE *f = fopen(nom, "r");
    i = 0;
    while((temp = fgetc(f)) != EOF)
    {
        res[i] = temp;
        i++;
    }
    res[i] = '\0';
    *n = i;
    fclose(f);
    return ;
}


void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    // char res[100000];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    // while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        n = Rio_readlineb(&rio, buf, MAXLINE);
        buf[n-1] = 0;
        printf("server received %u bytes\n", (unsigned int)n);
        int fd = open(buf, O_RDONLY);
        rio_t fileRio;
        rio_readinitb(&fileRio, fd);
        while((n = Rio_readlineb(&fileRio, buf, MAXLINE)) > 0){
            printf("hi: %ld\n", n);
            // buf[n] = 0;
            printf("%s\n", buf);
            Rio_writen(connfd, buf, n);
        }
        printf("end\n");
    // }
}

