/* 
 * File:   main.c
 * Author: Qbass
 *
 * Created on 17 kwiecień 2013, 19:18
 */
//CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int res;
struct srvdata{
        char servername[32];
        char serverip[16];
        char user[128];
        char password[128];
        int port;
        char command[128];
}data;
boolean portSet,serveripSet,servernameSet,commandSet,multiSet,userSet,passSet,uiModeSet;

int sendPacket(SOCKET sock, struct srvdata dt);
int sendCheck(SOCKET sock);
int sendLen(SOCKET sock, char* lenbuf);
int sendCmd(SOCKET sock, char* cmdbuf, int len);
void checkParams(int argc, char** argv);
int main(int argc, char** argv);

/*
 * 
 */


int sendPacket(SOCKET sock, struct srvdata dt){
    if(sendCmd(sock,(char*)&dt,sizeof(struct srvdata)) == -1){
        return -1;
    };
    if(sendCheck(sock) == -1){
        return -1;
    };
    return 0;
}

int sendCheck(SOCKET sock){
    char mesok[16];
    res = recv(sock,mesok,2,0);
    if(res == SOCKET_ERROR){
        return -1;
    }
    mesok[2] = '\0';
    if(strcmp(mesok,"OK") != 0){
        return -1;
    }
    return 0;
}

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

void checkParams(int argc, char** argv){ //maybe getopt.h?
    uiModeSet = 0;
    portSet = 0;
    serveripSet = 0;
    servernameSet = 0;
    commandSet = 0;
    multiSet = 0;
    userSet = 0;
    passSet = 0;
    strcpy(data.servername,".");
    strcpy(data.serverip,".");
    data.port = 0;
    int i;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i],"-ui") == 0){ //asking mode :P
            uiModeSet = 1;
        }
        else if(strcmp(argv[i],"-h") == 0){ //hostname
            strcpy(data.servername,argv[i+1]);
            servernameSet = 1;
        }else if(strcmp(argv[i],"-p") == 0){ //port
            data.port = atoi(argv[i+1]);
            portSet = 1;
        }else if(strcmp(argv[i],"-i") == 0){ //ipaddr
            strcpy(data.serverip,argv[i+1]);
            serveripSet = 1;        
        }else if(strcmp(argv[i],"-m") == 0){ //multi
            multiSet = 1;
        }else if(strcmp(argv[i],"-u") == 0){ //user
            strcpy(data.user,argv[i+1]);
            userSet = 1;
        }else if(strcmp(argv[i],"-ps") == 0){ //pass
            strcpy(data.password,argv[i+1]);
            passSet = 1;
        }else if(strcmp(argv[i],"-c") == 0){ //cmd
            strcpy(data.command,argv[i+1]);
            commandSet = 1;
        }
        
    }
}

int main(int argc, char** argv) {
    checkParams(argc, argv);
    if(uiModeSet){
        printf("(I)IP or (H)Hostname? ");
        char tmp;
        scanf("%c",&tmp);
        if(tmp == 'I'){
            printf("Server IP: ");
            scanf("%s",data.serverip);
            serveripSet = 1;
        }else{
            printf("Server Name: ");
            scanf("%s",data.servername);
            servernameSet = 1;
        }
        printf("Port: ");
        scanf("%d",&data.port);
        portSet = 1;
        printf("Username: ");
        scanf("%s",data.user);
        userSet = 1;
        printf("Password: ");
        scanf("%s",data.password);
        passSet = 1;
        printf("Command: ");
        scanf(" %[^\n]s",data.command);
        commandSet = 1;
        printf("CmdSpec: %s\n",data.command);
    }
    if(!(serveripSet || servernameSet)){
        printf("Server IP or Name not set!\n");
        exit(1);
    }
    if(!portSet){
        printf("Port not set!\n");
        exit(1);
    }
    printf("Server Data OK\n");
    if(!(userSet && passSet)){
        printf("User/Pass not specified!\n");
        exit(1);
    }
    printf("User/Pass set. Wait for login\n");
    if(!(commandSet || multiSet)){
        printf("Command not set or MultiMode not active!\n");
        exit(1);
    }
    printf("Command Parse OK\n");
    boolean ifip;
    if(strcmp(data.servername,".") == 0) ifip = 1;
    else if (strcmp(data.serverip,".") == 0) ifip = 0;
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
        adr.s_addr = inet_addr(data.serverip);
        he = gethostbyaddr((char *)&adr,4,AF_INET);
    }
    else he = gethostbyname(data.servername);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data.port);
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
        scanf("%s",data.command);
    }

    sendPacket(sockfd,data);
#ifdef debug
    printf("***Send packet ***\n");
    printf("ServerIP: %s\n",data.serverip);
    printf("ServerName: %s\n",data.servername);
    printf("ServerPort: %d\n",data.port);
    printf("User: %s\n",data.user);
    printf("Pass: %s\n",data.password);
    printf("Cmd: %s\n",data.command);
#endif
    printf("Executed %s without errors! :)",data.command);
    //system("pause");
    WSACleanup();
    
    return (EXIT_SUCCESS);
}

