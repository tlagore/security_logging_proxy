#include "proxy_server.h"

/* ProxyServer Constructor
 *
 * Initializes the ProxyServer object by giving it parameters to setup with
 * Sets up everything in the object to prepare it to be started with the 'startServer' method
 */
ProxyServer :: ProxyServer(int port, int logOption, char target[], int tartPort, int n){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }else{
    _ServerPort = DEFAULT_PORTNO;
  }
  memset(_ProxyOptions.targetName, 0, MAX_TARGET_SIZE);
  memcpy(_ProxyOptions.targetName, target, strlen(target));

  _ProxyOptions.targetName[strlen(target)] = '\0'; // added a null

  _ProxyOptions.targetPort = tartPort;
  _ProxyOptions.logOption = logOption;
  _ProxyOptions.autoN = n;
  _ProxyOptions.targetSocket = -1;
  
  _ServerSocket = -1;
}


/*
  startServer

  initializes the server socket then waits for a connection
*/
void ProxyServer :: startServer(){
  int enable_reuse = 1;
  int reusing;
					
  _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = htons(_ServerPort);
  _ServerAddress.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
  memset(_ServerAddress.sin_zero, '\0', sizeof _ServerAddress.sin_zero);
  
  // bind the socket with the set parameters
  bind(_ServerSocket, (struct sockaddr *) &_ServerAddress, sizeof(_ServerAddress));
  reusing = setsockopt(_ServerSocket, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(int));

  if(reusing < 0){
    cout << "!! Unable to reuse address";
  }
  
  // formatted output
  cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  cout << "!! Proxy Server Active" << endl;
  cout << "!! ~~~~~~~~~~~~~~~~~~~" << endl;
  cout << "!! config:" << endl;
  cout << "!! listening on ip: \t" << _ServerAddress.sin_addr.s_addr << endl;
  cout << "!! listening on port: \t" <<  _ServerPort << endl;
  cout << "!! dst ip: \t\t" << _ProxyOptions.targetName << endl;
  cout << "!! dst port: \t\t" << _ProxyOptions.targetPort << endl;
  switch(_ProxyOptions.logOption){
  case 0:
    cout << "!! logs: \t\t"<< "raw" << endl;      
    break;
  case 1:
    cout << "!! logs: \t\t"<< "strip" << endl;
      break;
  case 2:
    cout << "!! logs: \t\t"<< "hex" << endl;
    break;
  case 3:
    cout << "!! logs: \t\t"<< "auto" << _ProxyOptions.autoN << endl;
    break;
  case 4:
    cout << "!! logs: \t\t"<< "off" << endl;
  }
  cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  // start accepting connections...
  waitForConnection();
}

/*
  waitForConnection

  waitForConnection will listen for a connection on the server socket, but only if the server socket has been
  initialized. Once the client is connected, a message is sent to the client to confirm that the server is connected
  and the server waits for a command.
 */
void ProxyServer :: waitForConnection(){
  char buffer[1024];
  pthread_t worker;
  struct ProxyOptions *proxyOptionsCopy;
  printf("!! Accepting client connections...\n");
  while(_ServerSocket > -1){
    if(listen(_ServerSocket, 1) == 0){
    }

    proxyOptionsCopy = (struct ProxyOptions*)malloc(sizeof(_ProxyOptions));
    // added +1 to the length to copy below
    memcpy(proxyOptionsCopy->targetName, _ProxyOptions.targetName, strlen(_ProxyOptions.targetName)+1);
    proxyOptionsCopy->targetPort = _ProxyOptions.targetPort;
    proxyOptionsCopy->logOption = _ProxyOptions.logOption;
    proxyOptionsCopy->autoN = _ProxyOptions.autoN;
    proxyOptionsCopy->targetSocket = -1;

    /* accept call creates a new socket for incoming connection */
    _ClientAddrSize = sizeof _ClientStorage;
    
    proxyOptionsCopy->clientSocket = accept(_ServerSocket, (struct sockaddr *) &_ClientAddr, &_ClientAddrSize);
    
    printf("!! src ip: %s\n", inet_ntoa(_ClientAddr.sin_addr));
    
    pthread_create(&worker, NULL, &spawnWorker, (void*)proxyOptionsCopy);
  }
  printf("!! Init server socket first\n");
}




void ProxyServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }
}

int ProxyServer :: getPort(){
  return _ServerPort;
}
