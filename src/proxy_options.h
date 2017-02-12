#ifndef PROXY_OPTIONS_H
#define PROXY_OPTIONS_H

#define MAX_TARGET_SIZE 100

#define RAW 0
#define STRIP 1
#define HEX 2
#define AUTO_N 3
#define NONE 4

struct ProxyOptions{
  int clientSocket;
  int targetSocket;
  char targetName[MAX_TARGET_SIZE];
  int targetPort;
  int logOption;
  int autoN;
};

#endif
