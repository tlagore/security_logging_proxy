#include "proxy_master.h"


ProxyServer :: ProxyServer(int port, int logOption, char *target, int tartPort, int n){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }else{
    _ServerPort = DEFAULT_PORTNO;
  }

  _TargetName = (char*)calloc(1, strlen(target));
  memcpy(_TargetName, target, strlen(target));
  _TargetPort = tartPort;
  _LogOption = logOption;
  _AutoN = n;
  
  _ServerSocket = -1;
}


/*
  startServer

  initializes the server socket then waits for a connection
*/
void ProxyServer :: startServer(){

  _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = htons(_ServerPort);
  _ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(_ServerAddress.sin_zero, '\0', sizeof _ServerAddress.sin_zero);

  bind(_ServerSocket, (struct sockaddr *) &_ServerAddress, sizeof(_ServerAddress));

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
  int clientSocket;

  if (_ServerSocket > -1){
    if(listen(_ServerSocket, 1) == 0){
      printf("!! Ready to accept a connection...\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    clientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    printf("!! Client connected\n");

    strcpy(buffer, "\n-----------------------------------------------\n!! Server Connected\n");
    write(clientSocket, buffer, strlen(buffer));
    
    //once a connection has been made, wait for a command
  }else{
    printf("!! Init server socket first\n");
  }
}




void ProxyServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }
}

int ProxyServer :: getPort(){
  return _ServerPort;
}
