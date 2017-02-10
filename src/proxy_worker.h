/* proxy_worker.h */

#ifndef PROXY_WORKER_H
#define PROXY_WORKER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "proxy_options.h"

using namespace std;

#define MAX_TARGET_SIZE 100

class ProxyWorker{
 public:
  ProxyWorker(struct ProxyOptions *proxyOptions);
  ~ProxyWorker();
   
 private:
  int _ServerSocket;
  int _ClientSocket;
  
  struct ProxyOptions *_ProxyOptions;

  int _TargetSocket;
  
  struct sockaddr_in _TargetAddr;
  struct sockaddr_storage _TargetStorage;
  socklen_t _AddrSize;

  void initTargetSocket();
  void spawnClientListener();
  void spawnTargetListener();
 
  pthread_t _ClientReader;
  pthread_t _TargetReader;
  
  static void* listenClient(void *args){
    char buffer[2048];
    int amountRead;
    int* sockets = (int*)args;
    char prefix[] = "---> \0";

    amountRead = read(sockets[0], buffer, 2048);
    while(amountRead > 0){
      write(sockets[1], buffer, amountRead);
      logData(buffer, amountRead, prefix);
      amountRead = read(sockets[0], buffer, 2048);
    }
  }
  
  static void* listenTarget(void *args){
    char buffer[2048];
    int amountRead;
    int* sockets = (int*)args;
    char prefix[] = "<--- \0";

    amountRead = read(sockets[1], buffer, 2048);
    while(amountRead > 0){
      write(sockets[0], buffer, amountRead);
      logData(buffer, amountRead, prefix);
      amountRead = read(sockets[1], buffer, 2048);
    }
  }

  
  static void logData(char* buffer, int amountRead, char *prefix){
    int nextN = 0, previous = 0;
    while((nextN = nextNull(buffer, amountRead, previous)) != -1){
      printf("%s", prefix);
      char subbuff [(nextN-previous)];
      memset(subbuff, 0, nextN-previous);
      memcpy(subbuff, &buffer[previous], (nextN-previous));
      subbuff[nextN-previous] = '\0';
      printf("%s\n", subbuff); // print the lines
      if(buffer[nextN] == '\n' && nextN < amountRead - 1 && buffer[nextN+1] == '\0')
	previous = nextN + 2;
      else
	previous = nextN + 1;

      
    }
  }

  static int nextNull(char *buffer, int amountRead, int startingPoint){
    int i;
    for(i = startingPoint; i < amountRead; i++){
      if (buffer[i] == '\n' && i < amountRead - 1 && buffer[i+1] == '\0') // check for wierd case
	return i;
      if (buffer[i] == '\n')
	return i;
      if (buffer[i] == '\0')
	return i;
    }
    return -1;
  }

  
};

#endif
