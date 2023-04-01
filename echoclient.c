/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <string.h>
#include "time.h"
int file_available(int, rio_t, char*);
void save_file(char*, rio_t, int, int);


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


int main(int argc, char **argv){
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;
    FILE* file;

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
    
    int decalageLen = 0;
    if(p){
        printf("on reprend lecriture du fichier %s\n", p->nomFic);
        decalageLen = p->declage;
        Rio_writen(clientfd, &decalageLen, 4);
        p->nomFic[strlen(p->nomFic)] = '\n';
        if(!file_available(clientfd, rio, p->nomFic)){
            printf("file doesn't exist: buff: %s.\n", buf);
        }else{
            int i = 0;
            while (p->nomFic[i]!='\n')
                i++;
            p->nomFic[i] = 0;
            save_file(p->nomFic, rio, 1, clientfd);
        } 
    }
    else Rio_writen(clientfd, &decalageLen, 4);
    
    // tant que on n'as pas ecrit bye
    int bye = 0;
    while(!bye){ 
        char file_name[1000] = "./filesClient/";

        // get command from input
        Fgets(buf, MAXLINE, stdin);

        // exit if bye        
        if(!strcmp(buf, "bye\n")){
            printf("byed\n");
            bye = 1;
            continue;
        }
        
        // change \n to end 0
        int i = 0;
        while (buf[i]!='\n')
            i++;
        buf[i] = 0;
        
        strcat(file_name, buf);
        if((file = fopen(file_name, "r"))){
            printf("file already saved\n");
            printf("enter the next command\n\n");
            continue;
        }

        buf[i] = '\n';
        buf[i+1] = '\0';

        // send command to server
        if(!file_available(clientfd, rio, (char*)buf)){
            printf("file doesn't exist: buff: %s.\n", buf);
            continue;
        };

        // change \n to end 0
        buf[i] = 0;

        save_file(buf, rio, 0, clientfd);
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

void save_file(char* file, rio_t rio, int append, int clientfd){
    // file c'est le nom qu'on a envoye
    
    // get number of max_line
    char file_name[1000] = "./filesClient/";
    time_t start_t, end_t;
    double kb_sec;
    double diff_t;

    time(&start_t);

    int totalBytes = 0;
    strcat(file_name, file);
    int fd;
    if(append)
        fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);
    else
        fd = open(file_name, O_WRONLY | O_CREAT, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);

    // start the cache in case server stops immediately
    FILE* hiddenFile = NULL;
    hiddenFile = fopen(".cache", "w");
    fprintf(hiddenFile, "%d\n%s", 0, file);
    fclose(hiddenFile);

    // if server sends OK, then save_file()
    int packet_size, octs_lis=1;
    
    char *packet[MAXLINE];
    int i=0;
    int serverReponse = 1;
    // while (octs_lis > 0 && packet_size > 0) {
    while (serverReponse) {
        // read packet_size
        Rio_readnb(&rio, &packet_size, 4);
        if(packet_size == 0){
            // server crash
            printf("server ended upbrubtly\n");
            Close(fd);
            Close(clientfd);
            exit(0);
        }else if(packet_size == -1){
            serverReponse = 0;
            continue;
            // end of file
            // correct terminaison
        }

        hiddenFile = fopen(".cache", "w");
        fprintf(hiddenFile, "%d\n%s", i, file);
        fclose(hiddenFile);

        // read octs_lis
        octs_lis = Rio_readnb(&rio, &packet, packet_size);
        sleep(1);
        if(octs_lis == 0){
            // server crash
            serverReponse = 0;
            printf("server ended upbrubtly\n");
            Close(fd);
            Close(clientfd);
            exit(0);
        }
        printf("on lis %d octet\n", octs_lis);
        i++;
        
        Rio_writen(fd, packet, octs_lis);
        totalBytes += octs_lis;
    } 
    
    time(&end_t);
    diff_t = difftime(end_t, start_t);
    kb_sec = (totalBytes/1000)/diff_t;
    printf("\nTransfer successfully complete.\n");
    printf("%d bytes received in %f seconds(%f Kbytes/s)\n\n", totalBytes, diff_t, kb_sec);
    hiddenFile = fopen(".cache", "w");
    fprintf(hiddenFile, "succes");
    fclose(hiddenFile);
    Close(fd);
}