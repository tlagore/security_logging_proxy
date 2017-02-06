
#include "proxy_worker.h"

ProxyWorker::ProxyWorker(){
  
}

ProxyWorker::ProxyWorker(int clientSocket, char *hostName, int portNo, int log, int n){
  _LogMethod = log;
  _ClientSocket = clientSocket;
  _ServerAddr.sin_family = AF_INET;
  _ServerAddr.sin_port = htons(portNo);
  _ServerAddr.sin_addr.s_addr = inet_addr(hostName);

  try{
    memset(_ServerAddr.sin_zero, '\0', sizeof _ServerAddr.sin_zero);
    
    _AddrSize = sizeof _ServerAddr;
    connect(_ClientSocket, (struct sockaddr*)&_ServerAddr, _AddrSize);
  }catch(const std::exception &e){
    printf("Bad juju\n");
  }
}
