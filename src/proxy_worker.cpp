#include <errno.h>
#include "proxy_worker.h"
#include <ctime>

/*
  ProxyWorker constructor
  
  Receives a ProxyOptions struct which is assumed to hold an initialized client socket
  It then initializes a connection to the server specified in the ProxyOptions struct,
  spawns two slave threads to spin read/redirect traffic between the client and server,
  then waits for both threads to exit.
 */
ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;
  
  pid_t tid;
  tid = syscall(SYS_gettid);
  time_t  timev = time(nullptr);
  printf("!! [%d] ~ Client Connected - %s\n", tid, asctime(localtime(&timev)));

  initTargetSocket();

  spawnClientListener();
  spawnTargetListener();

  pthread_join(_ProxyOptions->clientThread, NULL);
  pthread_join(_ProxyOptions->targetThread, NULL);

  printf("!! Worker thread exited successfully\n");
}


/*
  initTargetSocket attempts to initialize the server socket with associated
  hostname and port number found in the ProxyOptions struct it received upon instantiation
 */
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


/*
  spawnClientListener spawns the client listener thread that will redirect input from the 
  client to the target
 */
void ProxyWorker::spawnClientListener(){
  int err = pthread_create(&(_ProxyOptions->clientThread), NULL, &listenClient, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Error initializing client listener\n");
  }
}

/*
  spawnClientListener spawns the client listener thread that will redirect input from the 
  client to the target
 */
void ProxyWorker::spawnTargetListener(){
  int err = pthread_create(&(_ProxyOptions->targetThread), NULL, &listenTarget, (void*)_ProxyOptions);
  if(err != 0){
    printf("!! Error initializing target listener\n");
  }
}

/*
  ProxyWorker Destructor, close sockets and free ProxyOption memory 
 */
ProxyWorker::~ProxyWorker(){
  close(_ProxyOptions->clientSocket);
  close(_ProxyOptions->targetSocket);
  free(_ProxyOptions);
}
