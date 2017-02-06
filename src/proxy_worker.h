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

#define RAW 0
#define STRIP 1
#define HEX 2
#define AUTO_N 3

class ProxyWorker{
 public:
  ProxyWorker();
  ProxyWorker(int clientSocket, char *hostName, int portNo, int log, int n);
 private:
  int _ClientSocket;
  int _ServerSocket;
  int _LogMethod;

  struct sockaddr_in _ServerAddr;
  socklen_t _AddrSize;
};

#endif
