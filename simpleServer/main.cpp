#include<io.h>
#include<stdio.h>
#include<winsock2.h>
//#define UDP

#define BUFLEN 100
#pragma comment(lib,"ws2_32.lib") //Winsock Library

 char count=0;
 char buf[200]="hello client";
  char recv_buf[200];
   int recv_len;
int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s , new_socket;

    //*****************
    struct sockaddr_in server , client;
   // struct sockaddr_in si_other;
    int c;
    char message[20];

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");
    //Create a TCP socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
#ifdef UDP
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("Socket created.\n");


    //Prepare the sockaddr_in structure
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("192.168.1.101");;
    client.sin_port = htons( 5000 );
#endif
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; //bind with machine address,local address
    server.sin_port = htons( 5000 );

#ifndef UDP

    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
    }

    puts("Bind done");

    //Listen to incoming connections
    listen(s , 3);
#endif
    printf("waiting for connection becoming available");

#ifndef UDP
while(1){
      while( (new_socket = accept(s , (struct sockaddr *)&client,&c))!= INVALID_SOCKET)
      {
          puts("Connection accepted");
          if (send(new_socket, buf, strlen(buf),0) == -1)
           {
              //   puts("Failed to send");
           }
           puts(recv_buf);

           //if we want to receive from ESP
           recv(new_socket,recv_buf, BUFLEN,0);
      }


      if (new_socket == INVALID_SOCKET)
          {
              printf("accept failed with error code : %d" , WSAGetLastError());
              return 1;
          }



#endif


#ifdef UDP
    if (sendto(new_socket, buf, 2, 0, (struct sockaddr*) &client, sizeof(client)) == -1)
     {
           puts("Failed to send");
     }
     puts(buf);
     recv_len=sizeof(client);
     recvfrom(new_socket,recv_buf, BUFLEN,0, (struct sockaddr *) &client, &recv_len);
#endif


   //wait for a second
    //Sleep(1000);
}
 closesocket(s);
 WSACleanup();

    return 0;
}

