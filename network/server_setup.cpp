#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>

#define MAXPENDING 5    /* Maximum outstanding connection requests */
using namespace std::chrono;

#define PORT 8000

high_resolution_clock::time_point getTime(){
  high_resolution_clock::time_point t = high_resolution_clock::now();
  return t;
}

double getTimeDif(const high_resolution_clock::time_point & t1, const high_resolution_clock::time_point & t2) {
   duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
   return time_span.count();
}

int main(int argc, char *argv[])
{
    std::cout << "Start Running Server" << std::endl;
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    
    if (argc == 2)     /* Test for correct number of arguments */
    {
        echoServPort = atoi(argv[1]);  /* First arg:  local port */
    } else {
        echoServPort = PORT;
    }
    
    
    
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))< 0) {
        std::cout << "Socket Creation Error" << std::endl;
    }
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr))< 0) {
        std::cout << "Bind Error" << std::endl;
    }
    
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0) {
        std::cout << "Listen Error" << std::endl;
    }
    
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                               &clntLen)) < 0) {
            std::cout << "Accept Failure " << std::endl;
        }
        
        /* clntSock is connected to a client! */
        
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    }
    /* NOT REACHED */
}
