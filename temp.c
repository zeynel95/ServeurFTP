/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

typedef struct paquet{
    int declage;
    char* nomFic;
} paquet;

paquet* getBeforeConnectInfo(){
    FILE* f = fopen(".cache","r");

    if(!f)
        return NULL;

    char *res = malloc(100);
    fscanf(f, "%s", res);
    
    if(!strcmp(res, "succes")){
        free(res);
        return NULL;
    }

    fclose(f);
    f = fopen(".cache", "r");
    paquet* p = malloc(sizeof(paquet));
    p->nomFic = malloc(100);
    fscanf(f, "%d\n%s", &p->declage, p->nomFic);
    fclose(f);
    return p;
}

void recupData(paquet* p, rio_t rio, char* res, int clientfd){
    Rio_writen(clientfd, p->nomFic, strlen(p->nomFic));
    int n;
    int fd = open(p->nomFic, O_WRONLY | O_CREAT, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH); 
    while ((n = Rio_readnb(&rio, res, MAXLINE)) > 0) {
        fprintf(stderr,"reconstitution paquet de %d bits recu!!\n", n);
        Rio_writen(fd, res, n);
    }
    Close(fd);

}

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
        paquet* p = getBeforeConnectInfo();
        if (p){
            recupData(p, rio, res, clientfd);
            Rio_readinitb(&rio, clientfd);
            
        }
        Fgets(buf, MAXLINE, stdin);
        Rio_writen(clientfd, buf, strlen(buf));
        int i = 0;
        while (buf[i]!='\n')
            i++;
        buf[i] = 0;
        int fd = open(buf, O_WRONLY | O_CREAT, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);
        FILE* hiddenFile = NULL;
        // perror("");
        i=0;
        while ((n = Rio_readnb(&rio, res, MAXLINE)) > 0) {
            // Fputs(buf, stdout);
            i++;
            fprintf(stderr,"paquet de %d bits recu!!\n", n);
            hiddenFile = fopen(".cache", "w");
            fprintf(hiddenFile, "%d\n%s", i, buf);
            fclose(hiddenFile);
            Rio_writen(fd, res, n);
            sleep(2);
        }

        hiddenFile = fopen(".cache", "w");
        fprintf(hiddenFile, "succes");
        Close(clientfd);
        Close(fd);
        exit(0);
}
