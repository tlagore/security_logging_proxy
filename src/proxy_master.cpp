#include "proxy_master.h"

ProxyServer :: ProxyServer(){
  _ServerSocket = -1;
}

ProxyServer :: ProxyServer(int port){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }else{
    _ServerPort = DEFAULT_PORTNO;
  }
  _ServerSocket = -1;
}

void ProxyServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }
}

int ProxyServer :: getPort(){
  return _ServerPort;
}

