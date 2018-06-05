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
#include <errno.h>
// #include <fcntl.h>
// #include <sys/mman.h>
// #include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

/*****************************************************
* Private Members Implementation:
*****************************************************/
int NWTest::createSocket(unsigned short servPort){
    // Create socket
    int servSock;
    if((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
         DieWithMessage("socket() failed");
    }

    return servSock;
}

void NWTest::bindSocket(int servSock, unsigned short servPort){
    // construct local address structure
    sockaddr_in servAddr;
    sockaddr * servAddr_t;

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);

    servAddr_t = (sockaddr *) &servAddr;
    //bind to local address
    if(bind(servSock, servAddr_t, sizeof(*servAddr_t)) < 0){
        std::string msg = "bind() failed: ";
        std::string error(strerror(errno));
        DieWithMessage(msg + error);
    }

    return;
}

void NWTest::HandleTCPClient(int clntSock){
    // TODO: to be implemented
    char buffer[BUFSIZE];
    ssize_t numBytesRcvd;
    std::cerr << "Server Received bytes" << std::endl;

    do{
      numBytesRcvd = recv(clntSock, buffer, BUFSIZE, 0);
      if(numBytesRcvd < 0){
        DieWithMessage("recv() failed");
      }
    }while(numBytesRcvd > 0);

    return;
}

void NWTest::connectSocket(int clntSock, std::string ip, unsigned short servPort){
    struct sockaddr_in servAddr;
    sockaddr * servAddr_t;

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    if(inet_pton(AF_INET, &ip[0], &servAddr.sin_addr.s_addr) <= 0){
        DieWithMessage("inet_pton() failed");
    }
    servAddr.sin_port = htons(servPort);
    servAddr_t = (sockaddr *) &servAddr;

    if(connect(clntSock, servAddr_t, sizeof(*servAddr_t)) < 0){
        DieWithMessage("connect() failed");
    }

    return;
}

void NWTest::bandWidthMeasurement(int servSock, uint64_t iter){
    char buffer[BUFSIZE];
    ssize_t numBytes;
    memset(&buffer, 0, sizeof(buffer));
    uint64_t total = 0;
    this->timer.warmUp();
    
    for(uint64_t i = 0; i < iter; i++){
        for(unsigned i = 0; i < TOTALSIZE; i++){
          uint64_t start = this->timer.getCpuCycle();
            if((numBytes = send(servSock, &buffer, sizeof(buffer), 0)) < 0){
                DieWithMessage("send() failed");
            }
          uint64_t end = this->timer.getCpuCycle();
          total += end - start;
        }
        double msec = this->timer.cycleToMsSec(total);
        double thruput = (double) (TOTALSIZE * BUFSIZE) / msec;
        this->res.push_back(thruput);
    }
}

int NWTest::setUpSocket(unsigned short servPort, int mode){
    int servSock = createSocket(servPort);
    switch(mode){
      case LISTEN:
        bindSocket(servSock, servPort);
        // set up socket as server
        if(listen(servSock, MAXPENDING) < 0){
          DieWithMessage("listen() failed");
        }

        std::cerr << "Server started at port: "<< servPort << std::endl;
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

      case SEND_REMOTE:
        // set up socket as client
        if(REMOTEIP.size() == 0){
          DieWithMessage("REMOTE IP not given, please specify in hpp");
        }
        connectSocket(servSock, REMOTEIP, servPort);
      break;

      case SEND_LOCAL:
        // send to localhost
        connectSocket(servSock, LOCALIP, servPort);
    }

    return servSock;
}


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
* Peak Network BandWdith bandwidth Implementation:
**************************************************************/

void NWTest::peakNetworkBandWidthServer(){
    // server
    setUpSocket(BW_TEST_PORT, LISTEN);
}

void NWTest::peakNetworkBandWidthTestLocal(uint64_t iter){
    // local
    resetNWTest();
    int servSock = setUpSocket(BW_TEST_PORT, SEND_LOCAL);
    bandWidthMeasurement(servSock, iter);
    close(servSock);
}

void NWTest::peakNetworkBandWidthTestRemote(uint64_t iter){
    // remote, remote ip is defined in hpp
    resetNWTest();
    int servSock = setUpSocket(BW_TEST_PORT, SEND_REMOTE);
    bandWidthMeasurement(servSock, iter);
    close(servSock);
}
