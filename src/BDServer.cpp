/*	
	BDServer.cpp
	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
	Tutorial: Tyrone-T01 James-T03
*/
#include "BDServer.h"

/*
  Default constructor, initiates a default port number
*/
BDServer :: BDServer(){
  _PortNo = DEFAULT_PORTNO;

  //initialize server socket and client socket to known state
  _ServerSocket = -1;
  _ClientSocket = -1;
}

/*
  Alternative constructor, given a port number it will set the specified
  port number as long as the port number falls in a valid range.

  Otherwise, default port is specified.
*/
BDServer :: BDServer(int portNo){
  if(portNo > 256 && portNo <= 65535){
    _PortNo = portNo;
  }else{
    _PortNo = DEFAULT_PORTNO;
  }

  //initialize server socket and client socket to known state
  _ServerSocket = -1;
  _ClientSocket = -1;

}

/*
  startServer

  initializes the server socket then waits for a connection
*/
void BDServer :: startServer(){

  _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = htons(_PortNo);
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
void BDServer :: waitForConnection(){
  char buffer[1024];

  if (_ServerSocket > -1){
    if(listen(_ServerSocket, 1) == 0){
      printf("!! Ready to accept a connection...\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    _ClientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    printf("!! Client connected\n");

    strcpy(buffer, "\n-----------------------------------------------\n!! Server Connected\n");
    write(_ClientSocket, buffer, strlen(buffer));
    
    //once a connection has been made, wait for a command
    waitForCommand();
  }else{
    printf("!! Init server socket first\n");
  }
}

/*
  waitForCommand

  waitForCommand assumes that a TCP connection has been made to a client.
  It then waits for a command to be sent from the client to parse and execute
  if the command is one that the sevrer can handle. If waitForCommand receives
  the string "off" or "help", then it handles the command directly, otherwise
  it hands the connection off to be parsed.
*/
void BDServer :: waitForCommand()
{
  int amountRead;
  char buffer[MAX_PATH];

  memset(buffer, 0, MAX_PATH);
  terminalLine();
  
  amountRead = read(_ClientSocket, buffer, MAX_PATH);
  while(amountRead > 0){
    trimLeft(buffer, strlen(buffer));
    trimRight(buffer, strlen(buffer));
    printf("<< %s\n", buffer);
    if (amountRead == 1){
      try{
	write(_ClientSocket, "-----------------------------------------------\n", 49);
	memcpy(buffer, "\n", 1);
	write(_ClientSocket, buffer, 1);
	memset(buffer, 0, MAX_PATH);
      }catch(const std::exception& e){
	//client has disconnected
	break;
      }
    }else if(strncmp(buffer, "off", strlen(buffer)) == 0){
      write(_ClientSocket, "-----------------------------------------------\n", 49);
      printf("!! 'off' command recieved. Shutting down.\n");
      memcpy(buffer, "!! Server shutting down. Goodbye!\n", 31);
      write(_ClientSocket, buffer, 31);
      close(_ClientSocket);
      close(_ServerSocket);
      return;
    }else if(strncmp(buffer, "help", strlen(buffer)) == 0){
      write(_ClientSocket, "-----------------------------------------------\n", 49);      
      helpMenu();
    }else{
      write(_ClientSocket, "-----------------------------------------------\n", 49);
      parseCommand(buffer);
      memset(buffer, 0, MAX_PATH);
    }

    memset(buffer, 0, MAX_PATH);
    printf("$$ Waiting for command...\n");
    terminalLine();
    
    amountRead = read(_ClientSocket, buffer, MAX_PATH);
  }

  printf("!! Client disconnected! Resetting client socket.\n");
  resetConnection();
  waitForConnection();
}


/*
  resetConnection

  resetConnection is called when a client disconnect is detected. It closes the current client
  socket and resets the client socket to a known state (for later error detection).
 */
void BDServer :: resetConnection(){
  close(_ClientSocket);

  _ClientSocket = -1;
}

/* UTIL Section
   
   We did not feel there were enough utility functions to warrent their own class just yet,
   however if this section grows - they will be moved to their own file
 */


/*
  trimLeft

  trims the whitespace to the left of the string and moves all characters into the 0 indexed position
 */
void BDServer :: trimLeft(char *input, int length){
  int index = 0, i = 0;

  while(index < length && (input[index] == ' ' || input[index] == '\n' || input[index] == '\t')){
    index ++;
  }

  while(i + index < length){
    input[i] = input[i + index];
    i++;
  }

  input[i] = '\0';
}

/*
  trimRight

  trims the whitespace to the right of the string and replaces the whitespace with null terminators
  to ensure easy usage later with functions such as strlen and strcmp
 */
void BDServer :: trimRight(char *input, int length){
  int index = length - 1;

  while(index > 0 && (input[index] == ' ' || input[index] == '\n' || input[index] == '\t')){
    input[index] = '\0';
    index --;
  }
}

/* Getters/Setters */
void BDServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _PortNo = port;
  }
}

int BDServer :: getPort(){
  return _PortNo;
}
/* End getters/Setters */
