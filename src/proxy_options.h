#ifndef PROXY_OPTIONS_H
#define PROXY_OPTIONS_H

#define MAX_TARGET_SIZE 100

struct ProxyOptions{
    int clientSocket;
    char targetName[MAX_TARGET_SIZE];
    int targetPort;
    int logOption;
    int autoN;
};

#endif
