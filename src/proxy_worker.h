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
  void listenClient();
  void listenTarget();

  void logData(char* buffcpy, int amountRead, char *prefix);
  int nextNull(char *buffer, int amountRead, int startingPoint); 
 
  pthread_t _ClientReader;
  pthread_t _TargetReader;
  
  static void* listenClient(void *args){
      char buffer[2048];
      int amountRead;
      int* sockets = (int*)args;

      amountRead = read(sockets[0], buffer, 2048);
      while(amountRead > 0){
	write(sockets[1], buffer, amountRead);
	amountRead = read(sockets[0], buffer, 2048);
      }
  }
  
  static void* listenTarget(void *args){
      char buffer[2048];
      int amountRead;
      int* sockets = (int*)args;

      amountRead = read(sockets[1], buffer, 2048);
      while(amountRead > 0){
	write(sockets[0], buffer, amountRead);
	amountRead = read(sockets[1], buffer, 2048);
      }
  }
  
};

#endif
