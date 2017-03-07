#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <errno.h>
#include <sstream>
#include <pthread.h>
#include <string>
#include <iostream>

using namespace std;

#define NUM_CLIENTS 514

int clients[NUM_CLIENTS];
int curClient= 0;

static void* listen_c(void*);
static void* chat_s(void*);

int main(int argc, char *argv[]){
  int s_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr, clientAddr;
  socklen_t clientSize;
  int port;
  int *client;
  pthread_t threads[1028];
  int i = 0;

  if(argc < 2){
    printf("Specify a port ya dummy\n");
  }else{
    port = atoi(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    bind(s_socket, (struct sockaddr*) &serverAddr, sizeof(serverAddr));


    printf("Entering loop...\n");
    while(s_socket > -1){
      //too many connections
      if(i >= 1025)
	break;
    
      if(listen(s_socket, 1) == 0)
	{
	}
      client = (int*)malloc(1 * sizeof(int));
      *client = accept(s_socket, (struct sockaddr *)&clientAddr, &clientSize);
      clients[curClient] = *client;
      printf("Got a client!\n");
    
      printf("Spawning listener, chatter\n");
      pthread_create(&threads[i], NULL, &listen_c, (void*)client);
      pthread_create(&threads[i+1], NULL, &chat_s, (void*)client);
      i+=2;
      curClient++;
    }

    printf("%d\n", s_socket);
  }
}


static void *listen_c(void* args){
  int *client = (int*) args;
  int amountRead;
  char buffer[1028];
  
  memset(buffer, 0, 1028);
  
  amountRead = read(*client, buffer, 1028);
  while(amountRead > 0){
    printf("%s", buffer);
    memset(buffer, 0, 1028);
    amountRead = read(*client, buffer, 1028);
  }
}

static void *chat_s(void* args){
  int amountRead;
  string buffer;

  cin >> buffer;
  while(strcmp(buffer.c_str(), "bye") != 0){
    buffer = buffer + "\n";
    for(int i = 0; i < curClient; i++){
      write(clients[i], buffer.c_str(), buffer.length());
    }
    cin >> buffer;
  }
  printf("leaving chat_s\n");
}
