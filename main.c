/* 
 * File:   main.c
 * Author: Qbass
 *
 * Created on 17 kwiecień 2013, 19:18
 */

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

/*
 * 
 */
int main(int argc, char** argv) {
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
    return (EXIT_SUCCESS);
}

