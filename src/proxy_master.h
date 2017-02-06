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

class ProxyServer{
public:
  ProxyServer();
  ProxyServer(int);
  void setPort(int port);
  int getPort();
private:
  int _ServerSocket;
  int _ServerPort;
};

#endif
