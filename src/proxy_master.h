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

#define RAW 0
#define STRIP 1
#define HEX 2
#define AUTO_N 3

class ProxyServer{
public:
  ProxyServer(int, int, char*, int, int);
  void startServer();

  void setPort(int port);
  int getPort();
private:
  int _ServerSocket;
  int _ServerPort;

  char* _TargetName;
  int _TargetPort;
  int _LogOption;
  int _AutoN;

  void waitForConnection();

  struct sockaddr_in _ServerAddress;
  struct sockaddr_storage _ServerStorage;
  socklen_t _AddrSize;
};

#endif
