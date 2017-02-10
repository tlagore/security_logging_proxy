#include <errno.h>
#include "proxy_worker.h"

ProxyWorker::ProxyWorker(struct ProxyOptions *proxyOptions){
  _ProxyOptions = proxyOptions;
  _ClientSocket = proxyOptions->clientSocket;
 
  initTargetSocket();
  spawnClientListener();
  listenTarget();
  spawnTargetListener();
  listenClient(); //need to know why we need this?


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
      printf("!! Worker: Error intializing connection to target server %d\n", _TargetSocket, errno);
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

void ProxyWorker::spawnTargetListener(){
  int *sockets = (int*)malloc(2*sizeof(int));
  sockets[0] = _ClientSocket;
  sockets[1] = _TargetSocket;

  printf("%d %d\n", sockets[0], sockets[1]);
  int err = pthread_create(&_TargetReader, NULL, &listenTarget, (void*)sockets);
  if(err != 0){
    printf("!! Worker: Error initializing target listener\n");
  }
}

void ProxyWorker::listenClient(){
  char buffer[2048];
  char buffcpy[2048];
  int amountRead;
  char prefix[] = "--> "; // need this without null
 
  printf("Listening to Client\n");
  
  amountRead = read(_ClientSocket, buffer, 2048);
  while(amountRead > 0){
    memcpy(buffcpy, buffer, amountRead);
    logData(buffcpy, amountRead, prefix);	  
    write(_TargetSocket, buffer, amountRead);
    amountRead = read(_ClientSocket, buffer, 2048);
  }
}

void ProxyWorker::listenTarget(){
  char buffer[2048];
  char buffcpy[2048];
  int amountRead;
  char prefix[] = "<-- "; // need this without null
 
  printf("Listening to target\n");
  
  amountRead = read(_TargetSocket, buffer, 2048);
  while(amountRead > 0){
    memcpy(buffcpy, buffer, amountRead);
    logData(buffcpy, amountRead, prefix);	  
    write(_ClientSocket, buffer, amountRead);
    amountRead = read(_TargetSocket, buffer, 2048);
  }
}

//we will ened to convert to user selected format
void ProxyWorker::logData(char* buffer, int amountRead, char *prefix){
  int nextN = 0, previous = 0;
  while((nextN = nextNull(buffer, amountRead, previous)) != -1){
    printf("%s", prefix);
    char subbuff [(nextN-previous)];
    memcpy(subbuff, &buffer[previous], (nextN-previous));
    printf("%s\n\0", subbuff); // print the lines
    printf("previous is %c\n\0", buffer[previous]);
    if(buffer[nextN] == '\n' && buffer[nextN+1] == '\0')
      previous = nextN + 2;
    else
      previous = nextN + 1;
  }
}



int ProxyWorker::nextNull(char *buffer, int amountRead, int startingPoint){
  int i;
  for(i = startingPoint; i < amountRead; i++){
    if (buffer[i] == '\n' && buffer[i+1] == '\0') // check for wierd case
      return i;
    if (buffer[i] == '\n')
      return i;
    if (buffer[i] == '\0')
      return i;
  }
  return -1;
}

ProxyWorker::~ProxyWorker(){
  printf("Worker: Exiting...\n");
  free(_ProxyOptions);
  close(_ClientSocket);
  close(_TargetSocket);
}
