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
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nwFsTest.hpp"
#include <fcntl.h>
#include <errno.h>

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

    if(::bind(servSock, servAddr_t, sizeof(*servAddr_t)) < 0){
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
    std::cerr << "Start Receiving packets" << std::endl;

    do{
      numBytesRcvd = recv(clntSock, buffer, BUFSIZE, MSG_WAITALL);
      if(numBytesRcvd < 0){
        DieWithMessage("recv() failed");
      }
    }while(numBytesRcvd > 0);
    log("Connection Closed");
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
        DieWithMessage("connect() failed, maybe forget to start local server? run ./nw_fs_test -s");
    }
    log("Connected");
    return;
}

void NWTest::bandWidthMeasurement(int servSock, uint64_t iter){
    char buffer[BUFSIZE];
    ssize_t numBytes;
    memset(&buffer, 0, sizeof(buffer));
    this->timer.warmUp();

    log("start sending data ...");
    for(uint64_t i = 0; i < iter; i++){
        log("Complete iteration " + std::to_string(i));
        uint64_t start = this->timer.getCpuCycle();
        for(unsigned i = 0; i < TOTALSIZE; i++){
            if((numBytes = send(servSock, &buffer, sizeof(buffer), 0)) < 0){
                DieWithMessage("send() failed");
            }
        }
        uint64_t end = this->timer.getCpuCycle();
        double msec = this->timer.cycleToMsSec(end - start);
        double thruput = (((double) (TOTALSIZE * BUFSIZE) / msec) / static_cast<double>(BYTE_TO_MBYTE)) * static_cast<double>(SEC_TO_MSEC);

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

/******************************************************
* Private file helpers:
*************************************************************/
int NWTest::openFileWithNoCache(std::string fileNameBase, bool isSequential, uint64_t fileSize){
    int fd;
    const uint64_t FILE_SIZE = fileSize * BYTE_TO_MBYTE;
    // Linux: open file, read / write directly to disk
    std::string fileName = fileNameBase + std::to_string(fileSize);
    // if((fd = open(&fileName[0], O_SYNC | O_CREAT | O_DIRECT | O_RDWR, S_IRUSR | S_IWUSR)) < 0){
    //   std::string error(strerror(errno));
    //   std::string msg = "open() failed: " + error;
    //   DieWithMessage(msg);
    // }

    // OSX: open
    if((fd = open(&fileName[0], O_SYNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0){
      std::string error(strerror(errno));
      std::string msg = "open() failed: " + error;
      DieWithMessage(msg);
    }

    if(ftruncate(fd, FILE_SIZE) < 0){
      std::string error(strerror(errno));
      std::string msg = "ftruncate() failed: " + error;
      DieWithMessage(msg);
    }

    // under Linux:
    // if(isSequential){
    //     if(posix_fadvise(fd, 0, FILE_SIZE, POSIX_FADV_DONTNEED | POSIX_FADV_SEQUENTIAL) < 0){
    //       std::string error(strerror(errno));
    //       std::string msg = "posix_fadvise() failed: " + error;
    //       DieWithMessage(msg);
    //     }
    // }else{
    //     if(posix_fadvise(fd, 0, FILE_SIZE, POSIX_FADV_DONTNEED | POSIX_FADV_RANDOM) < 0){
    //       std::string error(strerror(errno));
    //       std::string msg = "posix_fadvise() failed: " + error;
    //       DieWithMessage(msg);
    //     }
    // }

    //OSX: no cache
    if(fcntl(fd, F_NOCACHE, 1) < 0) {
        DieWithMessage("fcntl() failed");
    }

    return fd;
}

void NWTest::sequentialReadMeasurement(int fd, uint64_t iter, uint64_t fileSize){
    uint64_t start;
    uint64_t end;

    char buffer[FILE_BUFSIZE];
    this->timer.warmUp();

    for(unsigned i = 0; i < iter; i++){
        start = this->timer.getCpuCycle();
        while(read(fd, &buffer, sizeof(buffer)) > 0){}
        end = this->timer.getCpuCycle();
        double tm = this->timer.cycleToMsSec(end - start);
        double MbperMSec = tm / (double) fileSize;
        this->res.push_back(MbperMSec);

        // reset fd offset
        if(lseek(fd, 0, SEEK_SET) < 0){
            std::string error(strerror(errno));
            std::string msg = "lseek() failed: " + error;
            DieWithMessage(msg);
        }
    }
}

void NWTest::randomReadMeasurement(int fd,  uint64_t iter, uint64_t fileSize){
      uint64_t start;
      uint64_t end;
      const uint64_t FILE_SIZE = fileSize * BYTE_TO_MBYTE;
      char buffer[FILE_BUFSIZE];
      this->timer.warmUp();

      for(unsigned i = 0; i < iter; i++){

          start = this->timer.getCpuCycle();
          for(unsigned i = 0; i < (FILE_SIZE / FILE_BUFSIZE); i++){
            int seed = rand() % (FILE_SIZE / FILE_BUFSIZE);
            off_t pos = seed * FILE_BUFSIZE;
            read(fd, &buffer, sizeof(buffer));
            lseek(fd, pos, SEEK_SET);
          }
          end = this->timer.getCpuCycle();

          double tm = this->timer.cycleToMsSec(end - start);
          double MbperMSec = tm / (double) fileSize;
          this->res.push_back(MbperMSec);

          // reset fd offset
          if(lseek(fd, 0, SEEK_SET) < 0){
              std::string error(strerror(errno));
              std::string msg = "lseek() failed: " + error;
              DieWithMessage(msg);
          }
      }
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

void NWTest::fileSequentialReadTest(uint64_t iter, uint64_t fileSize){
    // set up file as non cache
    resetNWTest();
    int fd = openFileWithNoCache(TEST_FILENAME_BASE, true, fileSize);
    sequentialReadMeasurement(fd, iter, fileSize);
    close(fd);
}

void NWTest::fileRandomReadTest(uint64_t iter, uint64_t fileSize){
    resetNWTest();
    int fd = openFileWithNoCache(TEST_FILENAME_BASE, false, fileSize);
    randomReadMeasurement(fd, iter, fileSize);
    close(fd);
}
