#include <errno.h>
#include "proxy_worker.h"

ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;

  pid_t tid;
  tid = syscall(SYS_gettid);
  printf("!! [%d] ~ Client Connected\n", tid, tid);
  
  initTargetSocket();
  spawnClientListener();
  spawnTargetListener();

  pthread_join(_ProxyOptions->clientThread, NULL);
  pthread_join(_ProxyOptions->targetThread, NULL);
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
      printf("!! Error intializing connection to target server %d\n", errno);
    }
  }catch(const std::exception &e){
    printf("!! Error initializing connection to target server.\n");
  }
}

void ProxyWorker::spawnClientListener(){
  int err = pthread_create(&(_ProxyOptions->clientThread), NULL, &listenClient, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Error initializing client listener\n");
  }
}

void ProxyWorker::spawnTargetListener(){
  int err = pthread_create(&(_ProxyOptions->targetThread), NULL, &listenTarget, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Error initializing target listener\n");
  }
}

ProxyWorker::~ProxyWorker(){
  free(_ProxyOptions);
  close(_ProxyOptions->clientSocket);
  close(_ProxyOptions->targetSocket);
}
