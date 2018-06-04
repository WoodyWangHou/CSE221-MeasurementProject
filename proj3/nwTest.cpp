/******************************************************************************************
Project: UCSD CSE221 Course Project: CPU and OS Services Measurement

Author:
1. Hou Wang
2. Jinhao Tang
3. Zhenghong You

osservice.cpp:
implementation of testing procedures: context switching time measurement
*******************************************************************************************/
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nwTest.hpp"
// #include <sys/stat.h>
// #include <errno.h>
// #include <fcntl.h>
// #include <sys/mman.h>
// #include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

/*****************************************************
* Private Members Implementation:
*****************************************************/


/*****************************************************
* Public Members Implementation:
*****************************************************/


double NWTest::getStddev() {
    if(res.empty()){
        return 0.0;
    }

    double avg = this->getAvg();
  	double sd = 0.0;
  	for(double i : this->res){
  		  sd += pow(i - avg, 2.0);
  	}
  	sd /= static_cast<double>(this->res.size());
  	sd = pow(sd, 0.5);
  	return sd;
}

double NWTest::getAvg(){
    if(res.empty()){
        return 0.0;
    }

    double avg = 0.0;
    for(double i : this->res){
        avg += i;
    }
    avg /= static_cast<double>(this->res.size());
    return avg;
}


/*************************************************************
* Test memory read and write bandwidth Implementation:
**************************************************************/

int createSocket(int servPort){
    // Create socket
    int servSock;
    if((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
         DieWithMessage("socket() failed");
    }
}

void bindSocket(int servSock){
    // construct local address structure
    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);

    //bind to local address
    if(bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0){
        DieWithMessage("bind() failed");
    }

    return servSock;
}

void HandleTCPClient(){
    // TODO: to be implemented
}

void setUpSocket(int servPort, int mode){
    int servSock = createSocket(servPort);

    switch(mode){
      case LISTEN:
        bindSocket(servSock);
        // set up socket as server
        if(listen(servSock, MAXPENDING) < 0){
          DieWithMessage("listen() failed");
        }

        // server loop
        while(true){
          struct sockaddr_in clntAddr;
          socklen_t clntAddrLen = sizeof(clntAddr);

          int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
          if(clntSock < 0){
            DieWithMessage("accept() failed");
          }

          // clntSock set up, does nothing
          HandleTCPClient(clntSock);

          if(close(clntSock) < 0){
            DieWithMessage("close() failed");
          }
        }
      break;
      case SEND:
        // set up socket as client
        // TODO: to be implemented
      break;
    }
}

void setUpIdleTCPServer(int servPort){
    return setUpSocket(servPort, LISTEN);
}


void NWTest::peakBandwidthTest(uint64_t iter, std::string args){
    if(args.compare("-s") == 0){
      // server
      setUpIdleTCPServer(BW_TEST_PORT);
    }else{
      // client
      // TODO: to be implemented
    }
}
