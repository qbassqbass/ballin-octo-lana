/* 
 * File:   main.c
 * Author: Qbass
 *
 * Created on 17 kwiecień 2013, 19:18
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

/*
 * 
 */
int res;
char servername[32];
char serverip[16];
int port;
char command[128];
boolean portSet,serveripSet,servernameSet,commandSet,multiSet;

int sendLen(SOCKET sock, char* lenbuf){
    res = send(sock,lenbuf,sizeof(lenbuf),0);
    if(res == SOCKET_ERROR){
        return -1;
    }
    return 0;
}

int sendCmd(SOCKET sock, char* cmdbuf, int len){
    res = send(sock,cmdbuf,len,0);
    if(res == SOCKET_ERROR){
        return -1;
    }
    return 0;
}

void checkParams(int argc, char** argv){
    portSet = 0;
    serveripSet = 0;
    servernameSet = 0;
    commandSet = 0;
    multiSet = 0;
    strcpy(servername,".");
    strcpy(serverip,".");
    port = 0;
    int i;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i],"-h") == 0){ //hostname
            strcpy(servername,argv[i+1]);
            servernameSet = 1;
        }else if(strcmp(argv[i],"-p") == 0){ //port
            port = atoi(argv[i+1]);
            portSet = 1;
        }else if(strcmp(argv[i],"-i") == 0){ //ipaddr
            strcpy(serverip,argv[i+1]);
            serveripSet = 1;
        }else if(strcmp(argv[i],"-c") == 0){ //cmd
            strcpy(command,argv[i+1]);
            commandSet = 1;
        }else if(strcmp(argv[i],"-m") == 0 ){ //multi
            multiSet = 1;
        }
        
    }
}

int main(int argc, char** argv) {
    checkParams(argc, argv);
    if(serveripSet || servernameSet){
        if(portSet){
            
        }else{
            printf("Port not set!");
            exit(1);
        }
    }else{
        printf("Server IP or Name not set!");
        exit(1);
    }
    if(!(commandSet || multiSet)){
        printf("Command not set or MultiMode not active!");
        exit(1);
    }
    boolean ifip;
    if(strcmp(servername,".") == 0) ifip = 1;
    else if (strcmp(serverip,".") == 0) ifip = 0;
    else {
        printf("Host or IP Error");
        exit(1);
    }
    SOCKET sockfd;
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(result != NO_ERROR){
        perror("INIT");
    }
    printf("Winsock initialized\n");
    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("Socket");
        exit(1);
    }
    struct sockaddr_in addr;
    struct hostent *he;
    struct in_addr adr;
    if(ifip) {
        adr.s_addr = inet_addr(serverip);
        he = gethostbyaddr((char *)&adr,4,AF_INET);
    }
    else he = gethostbyname(servername);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
    res = connect(sockfd, (SOCKADDR *) &addr,sizeof(addr));
    if (res == SOCKET_ERROR){
        perror("Connect");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    char *clientip;
    clientip = inet_ntoa(addr.sin_addr);
    printf("Connected to %s\n",clientip);
    if(multiSet){
        printf("Command to process: ");
        scanf("%s",command);
    }
    int cmdSize = strlen(command);
    char sizebuf[16];
    sprintf(sizebuf,"%d",cmdSize);
    if(sendLen(sockfd,sizebuf) == -1){
        printf("Lenght sending failed with %u\n",WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    if(sendCmd(sockfd,command,cmdSize) == -1){
        printf("Command sending failed with %u\n",WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Executed %s without errors! :)",command);
    system("pause");
    WSACleanup();
    
    return (EXIT_SUCCESS);
}

