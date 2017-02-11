/* proxy_worker.h */

#ifndef PROXY_WORKER_H
#define PROXY_WORKER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "proxy_options.h"

using namespace std;

#define MAX_TARGET_SIZE 100

class ProxyWorker{
 public:
  ProxyWorker(struct ProxyOptions *proxyOptions);
  ~ProxyWorker();
   
 private:
  int _ServerSocket;
  int _ClientSocket;
  
  struct ProxyOptions *_ProxyOptions;

  int _TargetSocket;
  
  struct sockaddr_in _TargetAddr;
  struct sockaddr_storage _TargetStorage;
  socklen_t _AddrSize;

  void initTargetSocket();
  void spawnClientListener();
  void spawnTargetListener();
 
  pthread_t _ClientReader;
  pthread_t _TargetReader;
  
  static void* listenClient(void *args){
    char buffer[2048];
    int amountRead;
    ProxyOptions *proxyOptions = (ProxyOptions*)args;
    char prefix[] = "---> \0";

    amountRead = read(proxyOptions->clientSocket, buffer, 2048);
    while(amountRead > 0){
      write(proxyOptions->targetSocket, buffer, amountRead);
      logData(buffer, amountRead, proxyOptions->logOption, prefix);
      amountRead = read(proxyOptions->clientSocket, buffer, 2048);
    }
  }
  
  static void* listenTarget(void *args){
    char buffer[2048];
    int amountRead;
    ProxyOptions *proxyOptions = (ProxyOptions*)args;
    char prefix[] = "<--- \0";

    amountRead = read(proxyOptions->targetSocket, buffer, 2048);
    while(amountRead > 0){
      write(proxyOptions->clientSocket, buffer, amountRead);
      logData(buffer, amountRead, proxyOptions->logOption, prefix);
      amountRead = read(proxyOptions->targetSocket, buffer, 2048);
    }
  }

  static void logData(char* buffer, int amountRead, int logOption, char *prefix){
    switch(logOption){
    case RAW:
      logRaw(buffer, amountRead, prefix);
      break;
    case STRIP:
      strip(buffer, amountRead, '.');
      logRaw(buffer, amountRead, prefix);
      break;
    case HEX:
      logHex(buffer, amountRead, prefix);
      break;
    case AUTO_N:
      break;
    }
  }

  static void strip(char *buffer, int amountRead, char ch){
    int i;
    for(i = 0; i < amountRead;i++){
      if((buffer[i] < 32 || buffer[i] > 126) && buffer[i] != '\0' && buffer[i] != '\n')
	buffer[i] = ch;
    }
  }
  
  static void logRaw(char* buffer, int amountRead, char *prefix){
    int nextN = 0, previous = 0;
    while((nextN = nextNull(buffer, amountRead, previous)) != -1){
      printf("%s", prefix);
      char subbuff [(nextN-previous)];
      memset(subbuff, 0, nextN-previous);
      memcpy(subbuff, &buffer[previous], (nextN-previous));
      subbuff[nextN-previous] = '\0';
      printf("%s\n", subbuff); // print the lines
      if(buffer[nextN] == '\n' && nextN < amountRead - 1 && buffer[nextN+1] == '\0')
	previous = nextN + 2;
      else
	previous = nextN + 1;      
    }
  }

  /*
    This function is long and greasy
   */
  static void logHex(char *buffer, int amountRead, char *prefix){
    int i;
    char tmp[] = "|                   |\0";

    //print prefix + buffer address
    printf("\n%s%x ", prefix, buffer);

    for(i = 0; i <= amountRead; i++){
      printf("%02x ", buffer[i]);

      if(i != 0){
	//if we are at 16, at end of line - print ascii contents
	if(i % 16 == 0){
	  memcpy(tmp + 2, buffer + i - 16, 16);
	  for(int j = 0; j < 21; j++){
	    if(tmp[j] != 0x0A && tmp[j] != 0x0C && tmp[j] != 0x0D && tmp[j] != 0x00)
	      printf("%c", tmp[j]);
	    else
	      printf(" ");
	  }
	  printf("\n%s%x ", prefix, buffer + i);
	  memset(tmp + 2, 0, 16);

	  //else if we're at 8, print an extra space for 8 byte buffer
	}else if(i % 8 == 0){
	  printf(" ");
	}
      }else{
	//if we're at 0, print our first line
	for(int k = 0; k < 46; k++)
	  printf(" ");
	  
	for(int k =0; k < 21; k++){
	  if(tmp[k] != 0x0A)
	    printf("%c", tmp[k]);
	  else
	    printf(" ");

	}
	printf("\n%s%x ", prefix, buffer);
      }
    }
    
    memset(tmp + 2, 0, 16);
    
    //if we didn't finish a full line
    if(i % 16 != 0){
      memcpy(tmp + 2, buffer + i - (i % 16), (i % 16));

      //if we didn't get to 8, extra space buffer
      if(i % 16 < 8)
	printf(" ");

      //pad to buffer length
      for(int j = 0; j < 16; j++)
	tmp[i+2] = ' ';
      for(int j = i % 16; j <= 16; j++){
	printf("   ");
      }
      //print remaining contents
      for(int j = 0; j < 21; j++){
	
	if(tmp[j] != 0x0A && tmp[j] != 0x0C && tmp[j] != 0x0D && tmp[j] != 0x00)
	  printf("%c", tmp[j]);
	else
	  printf(" ");
      }
    }

    

    printf("\n");
  }

  static int nextNull(char *buffer, int amountRead, int startingPoint){
    int i;
    for(i = startingPoint; i < amountRead; i++){
      if (buffer[i] == '\n' && i < amountRead - 1 && buffer[i+1] == '\0') // check for wierd case
	return i;
      if (buffer[i] == '\n')
	return i;
      if (buffer[i] == '\0')
	return i;
    }
    return -1;
  }

  
};

#endif
