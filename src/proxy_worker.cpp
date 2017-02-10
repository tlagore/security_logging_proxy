#include <errno.h>
#include "proxy_worker.h"

ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;
 
  initTargetSocket();
  spawnClientListener();
  listenTarget();

  pthread_join(_ClientReader, NULL);
}


void ProxyWorker::initTargetSocket(){
  _TargetSocket = socket(PF_INET, SOCK_STREAM, 0);
    
  _TargetAddr.sin_family = AF_INET;
  _TargetAddr.sin_port = htons(_ProxyOptions->targetPort);
  _TargetAddr.sin_addr.s_addr = inet_addr(_ProxyOptions->targetName);
  int i;

  try{
    memset(_TargetAddr.sin_zero, '\0', sizeof _TargetAddr.sin_zero);
    
    _AddrSize = sizeof _TargetStorage;
    i = connect(_TargetSocket, (struct sockaddr*)&_TargetAddr, _AddrSize);
    
    if (i){
      printf("!! Worker: Error intializing connection to target server %d\n", _TargetSocket, i, errno);
    }
  }catch(const std::exception &e){
    printf("!! Worker: Error initializing connection to target server.\n");
  }
}

void ProxyWorker::spawnClientListener(){
  int *sockets = (int*)malloc(2*sizeof(int));
  sockets[0] = _ClientSocket;
  sockets[1] = _TargetSocket;

  printf("%d %d\n", sockets[0], sockets[1]);
  int err = pthread_create(&_ClientReader, NULL, &listenClient, (void*)sockets);
  if(err != 0){
    printf("!! Worker: Error initializing client listener\n");
  }
}


void ProxyWorker::listenTarget(){
  char buffer[2048];
  int amountRead;
 
  printf("Listening to target\n");
  
  amountRead = read(_TargetSocket, buffer, 2048);
  while(amountRead > 0){
    write(_ClientSocket, buffer, amountRead);
    amountRead = read(_TargetSocket, buffer, 2048);
  }
}


ProxyWorker::~ProxyWorker(){
  printf("Worker: Exitting...\n");
  free(_ProxyOptions);
  close(_ClientSocket);
  close(_TargetSocket);
}
