/* proxy_server.h */

#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

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

using namespace std;

class ProxyServer{
 public:
  ProxyServer();
 private:
  int _ServerSocket;
  int _ServerPortNo;
};

#endif
