#include "proxy_worker.h"


ProxyWorker::ProxyWorker(int clientSocket, char *hostName, int portNo, int log, int n){
  char buffer[2048];
  char words[] = "hello\n";
  ssize_t amountRead;
  _TargetSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  _LogMethod = log;
  _ClientSocket = clientSocket;
  _AutoN = n;
  _HostName = (char*)calloc(1, strlen(hostName));
  memcpy(_HostName, hostName, strlen(hostName));

  printf("Host: %s\n Port: %d\n", _HostName, portNo);
  
  _TargetAddr.sin_family = PF_INET;
  _TargetAddr.sin_port = htons(portNo);
  _TargetAddr.sin_addr.s_addr = inet_addr(_HostName);

  try{
    memset(_TargetAddr.sin_zero, '\0', sizeof _TargetAddr.sin_zero);
    
    _AddrSize = sizeof _TargetStorage;
    connect(_TargetSocket, (struct sockaddr*)&_TargetAddr, _AddrSize);

  }catch(const std::exception &e){
    printf("Bad juju\n");
  }
}

void ProxyWorker::readClient(){

}


void ProxyWorker::readTarget(){
  
}
