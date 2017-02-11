#include <errno.h>
#include "proxy_worker.h"

ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;
 
  initTargetSocket();
  spawnClientListener();
  spawnTargetListener();


  pthread_join(_TargetReader, NULL);
  pthread_join(_ClientReader, NULL);
}


void ProxyWorker::initTargetSocket(){
  _ProxyOptions->targetSocket = socket(PF_INET, SOCK_STREAM, 0);
    
  _TargetAddr.sin_family = AF_INET;
  _TargetAddr.sin_port = htons(_ProxyOptions->targetPort);
  _TargetAddr.sin_addr.s_addr = inet_addr(_ProxyOptions->targetName);
  int i;

  try{
    memset(_TargetAddr.sin_zero, '\0', sizeof _TargetAddr.sin_zero);
    
    _AddrSize = sizeof _TargetStorage;
    i = connect(_ProxyOptions->targetSocket, (struct sockaddr*)&_TargetAddr, _AddrSize);
    
    if (i){
      printf("!! Worker: Error intializing connection to target server %d\n", errno);
    }
  }catch(const std::exception &e){
    printf("!! Worker: Error initializing connection to target server.\n");
  }
}

void ProxyWorker::spawnClientListener(){
  int err = pthread_create(&_ClientReader, NULL, &listenClient, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Worker: Error initializing client listener\n");
  }
}

void ProxyWorker::spawnTargetListener(){
  int err = pthread_create(&_TargetReader, NULL, &listenTarget, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Worker: Error initializing target listener\n");
  }
}

//we will ened to convert to user selected format

ProxyWorker::~ProxyWorker(){
  printf("Worker: Exiting...\n");
  free(_ProxyOptions);
  close(_ProxyOptions->clientSocket);
  close(_ProxyOptions->targetSocket);
}
