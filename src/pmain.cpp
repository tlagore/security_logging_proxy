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

int main(int argc, char *argv[])
{
  int portNum;

  /* TODO handle extra args, consider using optarg */
  if (argc < 4){
    cout << "Invalid number of arguments. " << endl;
    cout << "Proper usage: " << endl << argv[0] << " [logOptions] srcPort server dstPort" << endl;
  }else{
    try{
      //arg format [options -raw -strip -hex -autoN] listenPort server serverPort
      if(argc == 4){
	//no option log specified
      }else{
	//log option specified
      }
      // portNum = stoi(argv[1]);
      // server.setPort(portNum);
      // server.startServer();
    }catch(const std::exception &e){
      cout << "Error in " << e.what();
    }
  }
}

void error(const char *errorMessage){
  perror(errorMessage);
  exit(1);
}
