/* proxy_server.h */

#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#define DEFAULT_PORTNO 2737
#define MAX_PATH 131072

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


using namespace std;

#include "proxy_worker.h"

#define RAW 0
#define STRIP 1
#define HEX 2
#define AUTO_N 3
#define MAX_TARGET_SIZE 100



class ProxyServer{
public:
  ProxyServer(int, int, char*, int, int);
  void startServer();

  void setPort(int port);
  int getPort();

  struct ProxyOptions{
    int clientSocket;
    char targetName[MAX_TARGET_SIZE];
    int targetPort;
    int logOption;
    int autoN;
  };
  
private:
  
  int _ServerSocket;
  int _ServerPort;

  struct ProxyOptions _ProxyOptions;
  /*
  char* _TargetName;
  int _TargetPort;
  int _LogOption;
  int _AutoN;*/

  void waitForConnection();

  struct sockaddr_in _ServerAddress;
  struct sockaddr_storage _ServerStorage;
  socklen_t _AddrSize;

  static void * spawnWorker(void * args){
    struct ProxyOptions *po = ((struct ProxyOptions*)args);
    printf("targetName:%s targetPort:%d logOption:%d n:%d cs:%d\n",
	   po->targetName, po->targetPort, po->logOption, po->autoN, po->clientSocket);
    //ProxyWorker pw(clientSocket, _TargetName, _TargetPort, _LogOption, _AutoN);


    //REMOVE THIS LINE, FREE IN PROXYWORKER
    free(po);
  }
};

#endif
