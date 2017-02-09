#include "proxy_worker.h"

ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;
  initTargetSocket();
  spawnClientListener();
  listenTarget();
}


void ProxyWorker::initTargetSocket(){
  _TargetSocket = socket(PF_INET, SOCK_STREAM, 0);
    
  _TargetAddr.sin_family = PF_INET;
  _TargetAddr.sin_port = htons(_ProxyOptions->targetPort);
  _TargetAddr.sin_addr.s_addr = inet_addr(_ProxyOptions->targetName);

  try{
    memset(_TargetAddr.sin_zero, '\0', sizeof _TargetAddr.sin_zero);
    
    _AddrSize = sizeof _TargetStorage;
    connect(_TargetSocket, (struct sockaddr*)&_TargetAddr, _AddrSize);
  }catch(const std::exception &e){
    printf("!! Worker: Error initializing connection to target server.\n");
  }
}

void ProxyWorker::spawnClientListener(){
  pthread_t clientReader;
  
  int err = pthread_create(&clientReader, NULL, &listenClient, NULL);
  if(err != 0){
    printf("!! Worker: Error initializing client listener\n");
  }
}


void ProxyWorker::listenTarget(){
  char buffer[2048];
  int amountRead;
  amountRead = read(_TargetSocket, buffer, 2048);
  while(amountRead > 0){
    printf("%s\n", buffer);
    
  }
  
  printf("Listening to target\n");
  
}


ProxyWorker::~ProxyWorker(){
  printf("Worker: Exitting...\n");
  free(_ProxyOptions);
  close(_ClientSocket);
  close(_TargetSocket);
}
