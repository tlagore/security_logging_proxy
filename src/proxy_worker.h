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

using namespace std;

#include "proxy_master.h"

class ProxyWorker{
 public:
  ProxyWorker(int clientSocket, char *hostName, int portNo, int log, int n);
 private:
  int _ClientSocket;
  int _ServerSocket;

  int _TargetSocket;
  
  int _LogMethod;
  int _AutoN;
  char *_HostName;

  struct sockaddr_in _TargetAddr;
  struct sockaddr_storage _TargetStorage;
  socklen_t _AddrSize;

  void listenTarget();
  
  static void* listenClient(void *args){
	  printf("I am readClient\n");
  }
};

#endif
