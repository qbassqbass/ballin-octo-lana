/* 
 * File:   main.c
 * Author: Qbass
 *
 * Created on 16 kwiecień 2013, 23:22
 */

#include <stdio.h>
#include <stdlib.h>
#include <w32api.h>
#include <winsock2.h>
#include <string.h>



/*
 * 
 */

#define CMD_ERROR = "CMD_255"

int res = 0;

int processCmd(char *cmd, char *u, char *p){
    //createprocess(cmd) with some user
    printf("Creating Process %s with %s:%s\n",cmd,u,p);
    return 0;
}

int recvLen(SOCKET sock){
    char messSize[16];
    res = recv(sock,messSize,sizeof(messSize),0);
    if(res == SOCKET_ERROR){
        return -1;
    }
    int s = atoi(messSize);
    return s;
}

int recvCmd(SOCKET sock, char* mess,int len){
    //receiving main message
    res = recv(sock,mess,len,0);
    if(res == SOCKET_ERROR){
        return -1;
    }
    mess[len] = '\0';
    return 0;
}

int recvUP(SOCKET sock, char* u, char* p){
    int ulen = 0;
    if((ulen = recvLen(sock) == -1)) {
        printf("UserLength Receive Error: %u\n",WSAGetLastError());
        return -1;
    }
    if(recvCmd(sock,u,ulen) == -1){
        printf("UserName Receive Error: %u\n",WSAGetLastError());
        return -1;
    }
    int plen = 0;
    if((plen = recvLen(sock) == -1)) {
        printf("PassLength Receive Error: %u\n",WSAGetLastError());
        return -1;
    }
    if(recvCmd(sock,p,ulen) == -1){
        printf("Password Receive Error: %u\n",WSAGetLastError());
        return -1;
    }    
}

int main(int argc, char** argv) {
    SOCKET sockfd;
    WSADATA wsaData;
    FILE *settings;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(result != NO_ERROR){
        perror("INIT");
    }
    printf("Winsock initialized\n");
    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("Socket");
        exit(1);
    }
    if((settings = fopen("settings.conf","r")) == NULL){
        printf("Error opening settings.conf file!\nExiting...\n");
        exit(1);
    }
    char tmp[64];
    char servip[16];
    int port;
    char portc[8];
    fscanf(settings,"%s",tmp);
    strcpy(servip,tmp+9);
    fseek(settings,13,1);
    fscanf(settings,"%s",portc);
    port = atoi(portc);
    printf("%s:%d\n",servip,port);
    struct sockaddr_in addr,aAddr;
    struct hostent *he;
    struct in_addr adr;
    adr.s_addr = inet_addr(servip);
    he = gethostbyaddr((char *)&adr,4,AF_INET);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
//    addr.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
    addr.sin_addr.s_addr = inet_addr(servip);
    int res;
    res = bind(sockfd,(SOCKADDR *)&addr,sizeof(addr));
    if(res == SOCKET_ERROR){
        printf("bind failed with %u\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }else{
        printf("*** Bind Successful ***\n");
    }
    if(listen(sockfd,SOMAXCONN) == -1) {
        perror("Listen");
        return 1;
    }else{
        printf("*** Listening on socket ***\n");
    }
    int acsize;
    SOCKET asockfd = accept(sockfd,(SOCKADDR *)&aAddr,&acsize);
    if (asockfd == -1){
        perror("Accept Socket");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    char *clientip;
    clientip = inet_ntoa(aAddr.sin_addr);
    printf("*** Client connected ***\n*** Client's address: %s ***\n",clientip);
    //receive user and password
    char user[128];
    char password[128];
    
    if(recvUP(asockfd, user, password) == -1){
        printf("Login error\n");
        closesocket(asockfd);
        //RETURN->LISTEN
    }
    
    //receiving length of next message
    int cmdLen = 0;
    if((cmdLen = recvLen(asockfd)) == -1){
        printf("Receiving length failed with %u\n",WSAGetLastError());
        closesocket(asockfd);
        // RETURN->LISTEN
    }

    char cmd[128];
    if(recvCmd(asockfd,cmd,cmdLen) == -1){
        printf("Receiving command failed with %u\n",WSAGetLastError());
        closesocket(asockfd);
    }
    //process command from char command[]
    
    return (EXIT_SUCCESS);
}

/*
    char messSize[16];
    res = recv(asockfd,&messSize,sizeof(messSize),0);
    if(res == SOCKET_ERROR){
        printf("Receiving length failed with %u\n",WSAGetLastError());
        closesocket(asockfd);
        closesocket(sockfd); //NOT SURE
        WSACleanup();
        return 1;
    }
    int s = atoi(messSize);
*/
/*
    char command[128];
    //receiving main message
    res = recv(asockfd,&command,cmdLen,0);
    if(res == SOCKET_ERROR){
        printf("Receiving command failed with %u\n",WSAGetLastError());
        closesocket(asockfd);
        closesocket(sockfd); //NOT SURE
        WSACleanup();
        return 1;
    }
    command[cmdLen] = '\0';
*/

