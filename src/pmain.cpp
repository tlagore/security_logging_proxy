/*	
	main for proxy program
	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
	Tutorial: Tyrone-T01 James-T03
*/

#include <string>
#include <iostream>
#include <regex>
#include <exception>

using namespace std;

#include "proxy_server.h"

void strip(char*, int);
int hostname_to_ip (char*, char*);

class ipexcept: public exception
{
  virtual const char* what() const throw()
  {
    return "Hostname Resolution\n";
  }
} ipexcept;

int main(int argc, char *argv[])
{
  int srcPort, dstPort;
  int n = -1, logOption;
  char *autoN;
  char hostip[16]; // max ip size + null
  /* TODO handle extra args, consider using optarg */
  if (argc < 4){
    cout << "!! Invalid number of arguments. " << endl;
    cout << "!! Proper usage: " << endl << argv[0] << " [logOptions] srcPort server dstPort" << endl;
  }else{
    try{
      //arg format [options -raw -strip -hex -autoN] listenPort server serverPort
      if(argc == 4){
	srcPort = stoi(argv[1]);
	dstPort = stoi(argv[3]);
	
	int resolution = hostname_to_ip(argv[2], hostip);
	
	if(!resolution){
	  ProxyServer pServer(srcPort, NONE, hostip, dstPort, -1);
	  pServer.startServer();
	}else{
	  throw ipexcept;
	}
      }else if(argc == 5){
	srcPort = stoi(argv[2]);
	dstPort = stoi(argv[4]);	
	if(strcmp(argv[1], "-raw") == 0){
	  logOption = RAW;
	  int resolution = hostname_to_ip(argv[3], hostip);

	  if(!resolution){
	    ProxyServer pServer(srcPort, RAW, hostip, dstPort, -1);
	    pServer.startServer();
	  }else{
	    throw ipexcept;
	  }
	}else if(strcmp(argv[1], "-strip") == 0){
	  logOption = STRIP;
	  int resolution = hostname_to_ip(argv[3], hostip);

	  if(!resolution){
	    ProxyServer pServer(srcPort, STRIP, hostip, dstPort, -1);
	    pServer.startServer();
	  }else{
	    throw ipexcept;
	  }
	}else if(strcmp(argv[1], "-hex") == 0){
	  logOption = HEX;
	  int resolution = hostname_to_ip(argv[3], hostip);

	  if(!resolution){
	    ProxyServer pServer(srcPort, HEX, hostip, dstPort, -1);
	    pServer.startServer();
	  }else{
	    throw ipexcept;
	  }
	}else if(strncmp(argv[1], "-auto", 5) == 0){
	  logOption = AUTO_N;
	  //+ 1 for null terminator
	  autoN = (char*)calloc((int)strlen(argv[1]) - 4, 0);
	  strncpy(autoN, argv[1] + 5, strlen(argv[1]) - 5);
	  autoN[(int)strlen(argv[1]) - 5] = '\0';
       
	  n = stoi(autoN);
	  int resolution = hostname_to_ip(argv[3], hostip);

	  if(!resolution){
	    ProxyServer pServer(srcPort, AUTO_N, hostip, dstPort, n);
	    pServer.startServer();
	  }else{
	    throw ipexcept;
	  }
	}else{
	  printf("!! Invalid log option.\n");
	}
      }else{
	cout << "!! Invalid number of arguments. " << endl;
	cout << "!! Proper usage: " << endl << argv[0] << " [logOptions] srcPort server dstPort" << endl;
      }
    }catch(const std::exception &e){
      cout << "!! Error in: " << e.what();
    }
  }
}

/*
 * Simple function to resolve hostnames to ip addresses
 * */
int hostname_to_ip (char *hostname, char *ip){

  if(regex_match(hostname, regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")) == 1){
    strcpy(ip, hostname);
    return 0; // already a valid ip address
  }
  
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *h;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(hostname, "http", &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next){
    h = (struct sockaddr_in *) p->ai_addr;
    strcpy(ip, inet_ntoa( h->sin_addr ));
  }
  printf("!! '%s' is '%s'\n", hostname, ip);
  freeaddrinfo(servinfo);
  return 0;
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
