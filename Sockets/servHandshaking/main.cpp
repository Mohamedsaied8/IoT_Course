#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET master , new_socket , client_socket[30] , s;
    struct sockaddr_in server, address;
    int max_clients = 30 , activity, addrlen, i, valread;
    char *message = "IP of ESP and control command received\r\n";

    //size of our receive buffer, this is string length.
    int MAXRECV = 1024;
    //set of socket descriptors
    fd_set readfds;
    //1 extra for null character, string termination
    char *buffer;
    buffer =  (char*) malloc((MAXRECV + 1) * sizeof(char));

    for(i = 0 ; i < 30;i++)
    {
        client_socket[i] = 0;
    }

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Initialised.\n");
    //Create a socket
    if((master = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5000 );

    //Bind
    if( bind(master ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    puts("Bind done");

    //Listen to incoming connections
    listen(master , 7);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    addrlen = sizeof(struct sockaddr_in);

    while(TRUE)
    {
        if ((s = accept(master , (struct sockaddr *)&address, (int *)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = recv( s , buffer, MAXRECV, 0);
        buffer[valread] = '\0';
        if(strstr(buffer,"$")!=0){
            printf("From Temperature");

             send(s , message , strlen(message) , 0 );
        }
        else{
             printf("From ECG");
        }



    }

}
