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
#include <sys/syscall.h>
#include <sys/types.h>

#include "proxy_options.h"

using namespace std;

#define MAX_TARGET_SIZE 100

/*
  ProxyWorker

  ProxyWorker handles a single client -> proxy -> server connection.
  It receives an initiated client connection and a specified target IP and host
  and initiates a connection to the server. Once both connections have been
  verified, the worker spawns two slave threads, one to listen to the client
  and one to listen to the server. Neither thread will halt on reads to allow
  for ending the thread in a graceful manner 
*/
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


  /**************************************************************************
   *                          STATIC FUNCTIONS                              *
   *                                                                        *
   *                  Functions used by slave threads                       *
   *************************************************************************/
  /* 
     listenClient receives a ProxyOptions struct and will spin read from the client,
     logging all output according to the ProxyOptions log options format, then redirecting
     the input to the target server.
     The loop runs until until 0 characters are read - signifying that the socket has closed. 
   */
  static void* listenClient(void *args){
    char buffer[2048];
    int amountRead;
    ProxyOptions *proxyOptions = (ProxyOptions*)args;
    char prefix[] = "---> \0";

    amountRead = read(proxyOptions->clientSocket, buffer, 2048);
    
    while(amountRead != 0){
      if(amountRead > 0){
	write(proxyOptions->targetSocket, buffer, amountRead);

	logData(buffer, amountRead, proxyOptions->logOption, prefix, proxyOptions->autoN);
	memset(buffer, 0, 2048);
      }
      amountRead = recv(proxyOptions->clientSocket, buffer, 2048, MSG_DONTWAIT);
    }

    pthread_cancel(proxyOptions->targetThread);
    printf("!! [%d] ~ Client listener exitting, sending interrupt to sister threads\n",
	   syscall(SYS_gettid));
  }


  /* 
     listenTarget receives a ProxyOptions struct and will spin read from the target,
     logging all output according to the ProxyOptions log options format, then redirecting
     the input to the client.
     The loop runs until until 0 characters are read - signifying that the socket has closed. 
   */
  static void* listenTarget(void *args){
    char buffer[2048];
    int amountRead;
    ProxyOptions *proxyOptions = (ProxyOptions*)args;
    char prefix[] = "<--- \0";

    amountRead = read(proxyOptions->targetSocket, buffer, 2048);
    
    while(amountRead != 0){
      if(amountRead > 0){
	write(proxyOptions->clientSocket, buffer, amountRead);
	
	logData(buffer, amountRead, proxyOptions->logOption, prefix, proxyOptions->autoN);
	memset(buffer, 0, 2048);
      }
      amountRead = recv(proxyOptions->targetSocket, buffer, 2048, MSG_DONTWAIT);
    }

    pthread_cancel(proxyOptions->clientThread);
    printf("!! [%d] ~ Target listener exitting, sending interrupt to sister threads\n",
	   syscall(SYS_gettid));
  }

  /*
    logData checks the log format and redirects to the appropriate log function
   */
  static void logData(char* buffer, int amountRead, int logOption, char *prefix, int autoN){
    switch(logOption){
    case NONE:
      break;
    case RAW:
      logRaw(buffer, amountRead, prefix);
      break;
    case STRIP:
      strip(buffer, amountRead, '.');
      logRaw(buffer, amountRead, prefix);
      break;
    case HEX:
      strip(buffer, amountRead, '.');
      logHex(buffer, amountRead, prefix);
      break;
    case AUTO_N:
      logAutoN(buffer, amountRead, prefix, autoN);
      break;
    default:
      break;
    }
  }


  /*
    strip receives a buffer, size of the buffer, and a character to replace
    it then replaces any character outside of the printable ascii range with the
    supplied character. 
   */
  static void strip(char *buffer, int amountRead, char ch){
    int i;
    for(i = 0; i < amountRead;i++){
      if((buffer[i] < 32 || buffer[i] > 126) && buffer[i] != '\0' && buffer[i] != '\n')
	buffer[i] = ch;
    }
  }

  /*
    logRaw takes in a buffer, amount read, and a prefix. It then 
    directly prints the output with the specified prefix.
   */
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
    logAutoN takes in a buffer, size of the buffer, prefix, and a value, 'n'.
    It then prints n bytes to stdout per line with the output prefixed with prefix
   */
  static void logAutoN(char * buffer, int amountRead, char *prefix, int n){
    int i;

    for (i = 0; i < (amountRead - n); i+=n){
      printf("%sBytes %d-%d%*.s", prefix, i, i+n, 10 - (numDigits(i) + numDigits(i + n)), " ");

      for(int j = i; j < (i + n); j++){
	printByte(buffer[j]);
      }
      printf("\n");
    }

    if(i != amountRead){
      printf("%sBytes %d-%d%*.s", prefix, i, amountRead, 10 - (numDigits(i) + numDigits(amountRead)), " ");
      for(i = i; i < amountRead; i++){
	printByte(buffer[i]);
      }
    }

    printf("\n");
  }


  /*
    printByte takes in a character and prints either the escaped character (ie, '\n' prints as '\n'),
    the associated ascii value, or the hex value if it falls outside of all previous checks
   */
  static void printByte(char ch){
    if(ch == '\\')
      printf("\\\\  ");
    else if(ch == '\n')
      printf("\\n  ");
    else if(ch == '\t')
      printf("\\t  ");
    else if(ch == '\r')
      printf("\\r  ");
    else if(ch >= 32 && ch <= 127)
      printf("%c   ", ch);
    else
      printf("\\%02X ", (unsigned char)ch);
  }

  /*
    numDigits checks the number of digits in a given integer. It does not discriminate negative
    or positive values;
   */
  static int numDigits(int n){
    int i = 1;
    n /= 10;
    while(n){
      i++;
      n /= 10;
    }
    return i;
  }
  
  /*
    logHex logs hex values to stdout in the format 
    "address" B1 B2 B3 B4 B5 B6 B7 B8  B9 B10 B11 B12 B13 B14 B16 | asciioutput..... |

    Output is identical to to hexdump -C.

    Note that address here is the address of the buffer containing the information, not the 
    address of where the information was retrieved from (ie the client or target server).
   */
  static void logHex(char *buffer, int amountRead, char *prefix){
    int i;
    char tmp[] = "|                   |\0";
    const int tmpSize = 21;
    const int width = 46;
    const int numHex = 16;

    //print prefix + buffer address
    printf("\n%s%x   ", prefix, buffer);

    for(i = 0; i < amountRead; i++){
      printf("%02X ", (unsigned char)buffer[i]);

      if(i != 0){
	//if we are at 16, at end of line - print ascii contents
	if(i % numHex == 0){
	  memcpy(tmp + 2, buffer + i - numHex, numHex);
	  for(int j = 0; j < tmpSize; j++){
	    if(tmp[j] != 0x0A && tmp[j] != 0x0C && tmp[j] != 0x0D && tmp[j] != 0x00)
	      printf("%c", tmp[j]);
	    else
	      printf(" ");
	  }
	  printf("\n%s%x   ", prefix, buffer + i);
	  memset(tmp + 2, 0, numHex);

	  //else if we're at 8, print an extra space for 8 byte buffer
	}else if(i % 8 == 0){
	  printf(" ");
	}
      }else{
	//if we're at 0, print our first line
	for(int k = 0; k < width; k++)
	  printf(" ");
	  
	for(int k =0; k < tmpSize; k++){
	  if(tmp[k] != 0x0A)
	    printf("%c", tmp[k]);
	  else
	    printf(" ");
	}
	printf("\n%s%x   ", prefix, buffer);
      }
    }
    
    memset(tmp + 2, 0, numHex);
    
    //if we didn't finish a full line
    if(i % 16 != 0){
      memcpy(tmp + 2, buffer + i - (i % 16), (i % 16));

      //if we didn't get to 8, extra space buffer
      if(i % numHex <= 8)
	printf(" ");

      //pad to buffer length
      for(int j = 2; j < numHex; j++)
	tmp[j] = ' ';
      for(int j = i % numHex; j <= numHex; j++){
	printf("   ");
      }
      //print remaining contents
      for(int j = 0; j < tmpSize; j++){
	if(tmp[j] != 0x0A && tmp[j] != 0x0C && tmp[j] != 0x0D && tmp[j] != 0x00)
	  printf("%c", tmp[j]);
	else
	  printf(" ");
      }
    }

    printf("\n");
  }

  /*
    nextNull looks for the nextNull character (or new line) and returns the index of
    this character.

    It starts searching from startingPoint and will not read past the buffer.
    
    If a character is not found, -1 is returned.
   */
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
