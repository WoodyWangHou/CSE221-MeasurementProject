#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define PORT  8000

using namespace std::chrono;

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
    
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoClntPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    
    if (argc == 2)     /* Test for correct number of arguments */
    {
        echoClntPort = atoi(argv[1]);  /* First arg:  local port */
    } else {
        echoClntPort = PORT;
    }

    
    /* Create socket for incoming connections */
    if ((clntSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))< 0) {
        std::cout << "Socket Creation Error" << std::endl;
    }
    
    /* Construct local address structure */
    memset(&echoClntAddr, 0, sizeof(echoClntAddr));   /* Zero out structure */
    echoClntAddr.sin_family = AF_INET;                /* Internet address family */
    echoClntAddr.sin_addr.s_addr = inet_addr("0.0.0.0"); //htonl(atoi("127.0.0.1")); /* Any incoming interface INADDR_ANY*/
    echoClntAddr.sin_port = htons(echoClntPort);      /* Local port */
    
    /* Connect to the Server */
    auto t1 = getTime();
    if (connect(clntSock, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    auto t2 = getTime();
    auto d = getTimeDif(t1,t2);
    std::cout << "Setup "<< d << std::endl;

    t1 = getTime();
    close(clntSock);
    t2 = getTime();
    d = getTimeDif(t1,t2);
    std::cout << "Tear Down "<< d << std::endl;
}
