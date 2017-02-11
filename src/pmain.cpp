/*	
	main for proxy program
	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
	Tutorial: Tyrone-T01 James-T03
*/

#include <string>
#include <iostream>
using namespace std;

#include "proxy_master.h"

void strip(char*, int);
int main(int argc, char *argv[])
{
  int srcPort, dstPort;
  int n = -1, logOption;
  char *autoN;

  /* TODO handle extra args, consider using optarg */
  if (argc < 4){
    cout << "Invalid number of arguments. " << endl;
    cout << "Proper usage: " << endl << argv[0] << " [logOptions] srcPort server dstPort" << endl;
  }else{
    try{
      //arg format [options -raw -strip -hex -autoN] listenPort server serverPort
      if(argc == 4){
	//no option log specified
	cout << "No log option specified, defaulting to raw output." << endl;
	srcPort = stoi(argv[1]);
	dstPort = stoi(argv[3]);

	ProxyServer pServer(srcPort, RAW, argv[2], dstPort, -1);
	pServer.startServer();
      }else if(argc == 5){
	srcPort = stoi(argv[2]);
	dstPort = stoi(argv[4]);
	
	if(strcmp(argv[1], "-raw") == 0){
	  logOption = RAW;
	  cout << "Raw output specified." << endl;
	  ProxyServer pServer(srcPort, RAW, argv[3], dstPort, -1);
	  pServer.startServer();
	}else if(strcmp(argv[1], "-strip") == 0){
	  logOption = STRIP;
	  cout << "Strip output specified." << endl;
	  ProxyServer pServer(srcPort, STRIP, argv[3], dstPort, -1);
	  pServer.startServer();
	}else if(strcmp(argv[1], "-hex") == 0){
	  cout << "Hex output specified." << endl;
	  logOption = HEX;
	  ProxyServer pServer(srcPort, HEX, argv[3], dstPort, -1);
	  pServer.startServer();
	}else if(strncmp(argv[1], "-auto", 5) == 0){
	  logOption = AUTO_N;
	  //+ 1 for null terminator
	  autoN = (char*)calloc((int)strlen(argv[1]) - 4, 0);
	  strncpy(autoN, argv[1] + 5, strlen(argv[1]) - 5);
	  autoN[(int)strlen(argv[1]) - 5] = '\0';
       
	  n = stoi(autoN);
	  printf("Auto %d output specified\n", n);

	  ProxyServer pServer(srcPort, AUTO_N, argv[3], dstPort, n);
	  pServer.startServer();
	}else{
	  printf("Invalid log option, defaulting to raw\n");
	}
      }else{
	cout << "Invalid number of arguments. " << endl;
	cout << "Proper usage: " << endl << argv[0] << " [logOptions] srcPort server dstPort" << endl;
      }
      // portNum = stoi(argv[1]);
      // server.setPort(portNum);
      // server.startServer();
    }catch(const std::exception &e){
      cout << "Error in " << e.what();
    }
  }
}


void strip(char * buffer, int amountRead){
  int i;
  for(i = 0; i < amountRead; i++){
    if(buffer[i] < 32 || buffer[i] > 126){
      buffer[i] = '\0';
    }
  }
}

void error(const char *errorMessage){
  perror(errorMessage);
  exit(1);
}
