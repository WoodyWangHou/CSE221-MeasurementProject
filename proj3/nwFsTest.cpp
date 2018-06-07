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
// Linux:
// #include <malloc.h>
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
using namespace std::chrono;

/*****************************************************
* Constructor Implementation:
*****************************************************/

high_resolution_clock::time_point getTime(){
  high_resolution_clock::time_point t = high_resolution_clock::now();
  return t;
}

double getTimeDif(const high_resolution_clock::time_point & t1, const high_resolution_clock::time_point & t2) {
   duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
   return time_span.count();
}

std::pair<double, double> NWTest::setUpTearLocal(){
  resetNWTest();
  int servSock = setUpSocket(BW_TEST_PORT, SEND_LOCAL);
  auto t1 = getTime();
  close(servSock);
  auto t2 = getTime();
  testTearDown = getTimeDif(t1,t2);
  return {testSetup, testTearDown};
}

std::pair<double, double> NWTest::setUpTearRemote(){
  resetNWTest();
  int servSock = setUpSocket(BW_TEST_PORT, SEND_REMOTE);
  auto t1 = getTime();
  close(servSock);
  auto t2 = getTime();
  testTearDown = getTimeDif(t1,t2);
  return {testSetup, testTearDown};
}


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
    ssize_t numBytes = 0;
    memset(buffer, 0, sizeof(buffer));
    this->timer.warmUp();

    log("start sending data ...");
    for(uint64_t i = 0; i < iter; i++){
        log("Complete iteration " + std::to_string(i));
        uint64_t start = this->timer.getCpuCycle();
        for(unsigned i = 0; i < TOTALSIZE; i++){
            if((numBytes = send(servSock, buffer, sizeof(buffer), 0)) < 0){
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
    auto t1 = getTime();
    auto t2 = getTime();
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

        t1 = getTime();
        connectSocket(servSock, REMOTEIP, servPort);
        t2 = getTime();
        testSetup = getTimeDif(t1,t2);
      break;

      case SEND_LOCAL:
        // send to localhost
        t1 = getTime();
        connectSocket(servSock, LOCALIP, servPort);
        t2 = getTime();
        testSetup = getTimeDif(t1,t2);
    }

    return servSock;
}

/******************************************************
* Private file helpers:
*************************************************************/
int NWTest::openFileWithNoCache(std::string fileNameBase, uint64_t fileSize){
    int fd;
    const uint64_t FILE_SIZE = fileSize * BYTE_TO_MBYTE;
    // Linux: open file, read / write directly to disk
    std::string fileName = fileNameBase + std::to_string(fileSize);

    // Linux:
    // if((fd = open(&fileName[0], O_CREAT | O_RDWR | O_DIRECT, S_IRUSR | S_IWUSR)) < 0){
    //   std::string error(strerror(errno));
    //   std::string msg = "open() failed: " + error;
    //   DieWithMessage(msg);
    // }

    // // OSX: open
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

    // if(posix_fadvise(fd, 0, FILE_SIZE, POSIX_FADV_DONTNEED) < 0){
    //   std::string error(strerror(errno));
    //   std::string msg = "posix_fadvise() failed: " + error;
    //   DieWithMessage(msg);
    // }

    // OSX: no cache
    if(fcntl(fd, F_NOCACHE, 1) < 0) {
        DieWithMessage("fcntl() failed");
    }
    return fd;
}

void NWTest::sequentialReadMeasurement(int fd, uint64_t iter, uint64_t fileSize){
    uint64_t start;
    uint64_t end;
    // Linux:
    // char* buffer = (char*) memalign(FILE_BUFSIZE, FILE_BUFSIZE);

    //OSX:
    char buffer[FILE_BUFSIZE];
    memset(buffer, 0, FILE_BUFSIZE);
    this->timer.warmUp();

    for(unsigned i = 0; i < iter; i++){
        start = this->timer.getCpuCycle();
        while((read(fd, buffer, FILE_BUFSIZE)) > 0){}
        end = this->timer.getCpuCycle();
        double tm = this->timer.cycleToMsSec(end - start);
        this->res.push_back(tm);

        // reset fd offset
        //Linux:
        // if(lseek64(fd, 0, SEEK_SET) < 0){

        // OSX:
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
      // Linux:
      // char* buffer = (char*) memalign(FILE_BUFSIZE, FILE_BUFSIZE);

      //OSX:
      char buffer[FILE_BUFSIZE];
      memset(buffer, 0, FILE_BUFSIZE);
      this->timer.warmUp();

      for(unsigned i = 0; i < iter; i++){

          start = this->timer.getCpuCycle();
          for(unsigned i = 0; i < (FILE_SIZE / FILE_BUFSIZE); i++){
            read(fd, buffer, FILE_BUFSIZE);
            //OSX:
            lseek(fd, rand() % (FILE_SIZE / FILE_BUFSIZE) * FILE_BUFSIZE, SEEK_SET);
            // Linux:
            // lseek64(fd, rand() % (FILE_SIZE / FILE_BUFSIZE) * FILE_BUFSIZE, SEEK_SET);
          }
          end = this->timer.getCpuCycle();
          double tm = this->timer.cycleToMsSec(end - start);
          this->res.push_back(tm);

          // reset fd offset
          // Linux:
          // if(lseek64(fd, 0, SEEK_SET) < 0){

          // OSX:
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
    int fd = openFileWithNoCache(TEST_FILENAME_BASE, fileSize);
    sequentialReadMeasurement(fd, iter, fileSize);
    close(fd);
}

void NWTest::fileRandomReadTest(uint64_t iter, uint64_t fileSize){
    resetNWTest();
    int fd = openFileWithNoCache(TEST_FILENAME_BASE, fileSize);
    randomReadMeasurement(fd, iter, fileSize);
    close(fd);
}
