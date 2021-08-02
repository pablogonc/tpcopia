#ifndef tcp_server_c
#define tcp_server_c

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "tcp_server.h"
 
int iniciar_servidor(char * ip,char * puerto){
    int socket_servidor , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_servidor = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_servidor == -1)
    {
        printf("Could not create socket");
    }
    if(DEBUG_MODE){
    	puts("Socket created");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons((u_short) strtoul(puerto,NULL,0));
     
    //Bind
    if( bind(socket_servidor,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Bind failed. Error");
        return 1;
    }
    if(DEBUG_MODE){
        puts("Bind done");
    }

    
    //Listen
    listen(socket_servidor , 30); //numero magico
     
    //Accept and incoming connection
    if(DEBUG_MODE){
        puts("Waiting for incoming connections...");
    }

    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while(1)
    {
    	sem_wait(socket1);
    	int client_sock;
        (client_sock = accept(socket_servidor, (struct sockaddr *)&client, (socklen_t*)&c));
        if(DEBUG_MODE){
            puts("Connection accepted");
        }

        if (client_sock < 0)
        {
            perror("Accept failed");
            return 1;
        }
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        if(DEBUG_MODE){
            puts("Handler assigned");

        }
    }
    
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    
    escuchar(sock);
    
    close(sock);

    return 0;
} 

#endif
