/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include <string.h>



void echo(int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    
    Rio_readinitb(&rio, connfd);

    /******************************/
    int DecalageLen;
    Rio_readnb(&rio, &DecalageLen, 4);
    // si il y a un fichier a recuperer
    if (DecalageLen)
    {
        printf("on reprend lecriture du fichier\n");
        n = Rio_readlineb(&rio, buf, MAXLINE);
        char file_name[1000] = "./filesServeur/";
        printf("communication ready from server\n");
        buf[n-1] = 0;
        printf("file gotten: %s.\n", buf);
        printf("server received %u bytes\n", (unsigned int)n);

        strcat(file_name, buf);

        // if fichier n'existe pas
        FILE *file;
        if (!(file = fopen(file_name, "r"))){
            printf("Fichier n'existe pas\n");
            int response = 0;
            Rio_writen(connfd, &response, 4);
            // continue;
        } else{
            printf("Fichier existe\n");
            int response = 1;
            Rio_writen(connfd, &response, 4);


            int fd = open(file_name, O_RDONLY);
            rio_t fileRio;
            rio_readinitb(&fileRio, fd);
            int i = 1;
            while((n = Rio_readnb(&fileRio, file_name, MAXLINE)) > 0){
                if (i>DecalageLen)
                {
                    Rio_writen(connfd, &n, 4);
                    Rio_writen(connfd, file_name, n);
                }
                i++;
            }
        }
    }
    
    /******************************/
    
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) > 0){
        char file_name[1000] = "./filesServeur/";
        printf("communication ready from server\n");
        
        buf[n-1] = 0;
        printf("file gotten: %s.\n", buf);
        printf("server received %u bytes\n", (unsigned int)n);

        strcat(file_name, buf);

        // if fichier n'existe pas
        FILE *file;
        if (!(file = fopen(file_name, "r"))){
            printf("Fichier n'existe pas\n");
            int response = 0;
            Rio_writen(connfd, &response, 4);
            continue;
        } else{
            printf("Fichier existe\n");
            int response = 1;
            Rio_writen(connfd, &response, 4);
        }


        int fd = open(file_name, O_RDONLY);
        rio_t fileRio;
        rio_readinitb(&fileRio, fd);

        while((n = Rio_readnb(&fileRio, file_name, MAXLINE)) > 0){

            Rio_writen(connfd, &n, 4);
            Rio_writen(connfd, file_name, n);
            // getchar();
        }
    }
}