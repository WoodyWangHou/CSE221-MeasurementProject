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
  int t = 1;
  close(servSock);
  setsockopt(servSock,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
  //shutdown(servSock, 2);
  auto t2 = getTime();
  testTearDown = getTimeDif(t1,t2);
  return {testSetup, testTearDown};
}

std::pair<double, double> NWTest::setUpTearRemote(){
  resetNWTest();
  int servSock = setUpSocket(BW_TEST_PORT, SEND_REMOTE);
  auto t1 = getTime();
  int t = 1;
  close(servSock);
  setsockopt(servSock,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
  //shutdown(servSock, 2);
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

void NWTest::handleEcho(int clntSock) {
    char buffer[PING_SZ];
    ssize_t numBytesRcvd;
    std::cerr << "Start Receiving packets" << std::endl;

    do{
      numBytesRcvd = recv(clntSock, buffer, PING_SZ, MSG_WAITALL);
      if(numBytesRcvd < 0){
        DieWithMessage("recv() failed");
      }
      int numBytesWrite = 0;
      if((numBytesWrite = write(clntSock, buffer, PING_SZ)) < 0) {
          DieWithMessage("recv() failed");
      }
      std::cout << "echo back " << numBytesWrite << " bytes" << std::endl;

    } while(numBytesRcvd > 0);
    log("Connection Closed");
    return;
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
      case PING:
        bindSocket(servSock, servPort);
        // set up socket as server
        if(listen(servSock, MAXPENDING) < 0){
          DieWithMessage("listen() failed");
        }

        std::cerr << "Echo Test Server started at port: "<< servPort << std::endl;
        // server loop
        while(true){
          struct sockaddr_in clntAddr;
          socklen_t clntAddrLen = sizeof(clntAddr);

          int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
          if(clntSock < 0){
            DieWithMessage("accept() failed");
          }

          // clntSock set up, does nothing
          handleEcho(clntSock);

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
    // if(fcntl(fd, F_NOCACHE, 1) < 0) {
    //     DieWithMessage("fcntl() failed");
    // }
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
* Ping Test Implementation:
**************************************************************/




int NWTest::createPingSocket() {

    int sock;
    int opt = 1;

    if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) 
      DieWithMessage("create raw socket error");

    if (setsockopt(sock, SOL_SOCKET, 
      SO_REUSEPORT, &opt, sizeof(opt)))
    DieWithMessage("setsockopt() failed");

    // if ( fcntl(sock, F_SETFL, O_NONBLOCK) != 0 )
    //     DieWithMessage("Request nonblocking I/O");

    return sock;
}



void NWTest::pingTestServer() {
    setUpSocket(PING_TEST_SERVER_PORT, PING);
}




void NWTest::sendToSrc(unsigned char buf[], int sz, struct sockaddr_in src) {
    cout << "Received icmp message" << sz << " bytes...." <<"ready to reply back" << endl;
}


void NWTest::ping(struct sockaddr_in addr, int sock, int mode) {

    struct sockaddr_in r_addr;
    struct icmp icmp_hdr;
    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    

    for (;;) { 

        
        
        unsigned char data[PACKET_SIZE];
        socklen_t len=sizeof(r_addr);
        int rc;
        break;
        memset(data + sizeof(icmp_hdr), 0, PACKET_SIZE - sizeof(icmp_hdr)); //icmp payload
        icmp_hdr.icmp_type = ICMP_ECHO;
        icmp_hdr.icmp_cksum = checksum(data, PACKET_SIZE);
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);

        if((rc = sendto(sock, data, PACKET_SIZE,
                        0, (struct sockaddr*)&addr, sizeof addr)) <= 0) 
          DieWithMessage("sendto");

        std::cout << "data sent....waiting for recv" << inet_ntoa(addr.sin_addr) << std::endl;

        if ((rc=recvfrom(sock, data, PACKET_SIZE, 0, (struct sockaddr*)&r_addr, &len) > 0 )) {
            cout << rc << "bytes from " << inet_ntoa(r_addr.sin_addr) << ": ";
            break;
        } 
    }
    if (mode == 0) {
        p("ping -c 10 127.0.0.1");
    } else {
        p("ping -c 10 52.25.77.52");
    }
    
}

void NWTest::p(char* cmd) {
    system(cmd);
    return;
}

void NWTest::p() {
    system("ping -c 5 127.0.0.1");
    return;
}



struct sockaddr_in NWTest::srcAddr(string ip) {
    struct sockaddr_in addr;

    if(inet_pton(AF_INET, &ip[0], &addr.sin_addr.s_addr) <= 0){
        DieWithMessage("inet_pton() failed");
    }
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    return addr;
}



void NWTest::testPingLocal(uint64_t iter) {
    resetNWTest();

    
    int sock;
    struct sockaddr_in src_addr;
    char buffer[BUFSIZE];
    char rcvBuf[BUFSIZE];
    memset(buffer, 0, BUFSIZE);

    sock = setUpSocket(PING_TEST_SERVER_PORT, SEND_LOCAL);
    int bytesWrite = 0;
    int bytesRead = 0;

    this->timer.warmUp();
    for (int i = 0; i < iter; i++) {
        uint64_t start = this->timer.getCpuCycle();
        
        if ((bytesWrite = write(sock, buffer, PING_SZ)) < 0) {
            DieWithMessage("echo write failed on client");
        }


        if ((bytesRead = read(sock, rcvBuf, PING_SZ)) < 0) {
            DieWithMessage("echo write failed on client");
        }
        uint64_t end = this->timer.getCpuCycle();
        double msec = this->timer.cycleToMsSec(end - start);
        cout <<  "echo " << bytesRead << "bytes......" << "time= "<< msec << " ms" << endl;
        this->res.push_back(msec);
    }

    cout << "min/std   " << getAvg() << "/" << getStddev()<< endl;
}



void NWTest::testPingRemote(uint64_t iter) {
    resetNWTest();
    
    
    int sock;
    struct sockaddr_in src_addr;
    char buffer[BUFSIZE];
    char rcvBuf[BUFSIZE];
    memset(buffer, 0, BUFSIZE);

    sock = setUpSocket(PING_TEST_SERVER_PORT, SEND_REMOTE);
    int bytesWrite = 0;
    int bytesRead = 0;

    this->timer.warmUp();
    for (int i = 0; i < iter; i++) {
        uint64_t start = this->timer.getCpuCycle();
        
        if ((bytesWrite = write(sock, buffer, PING_SZ)) < 0) {
            DieWithMessage("echo write failed on client");
        }


        if ((bytesRead = read(sock, rcvBuf, PING_SZ)) < 0) {
            DieWithMessage("echo write failed on client");
        }
        uint64_t end = this->timer.getCpuCycle();
        double msec = this->timer.cycleToMsSec(end - start);
        cout <<  "echo " << bytesRead << "bytes......" << "time= "<< msec << " ms" << endl;
        this->res.push_back(msec);
    }

    cout << "min/std   " << getAvg() << "/" << getStddev()<< endl;
}



unsigned short NWTest::checksum(void *b, int len){ 
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
      sum += *buf++;
    if ( len == 1 )
      sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void NWTest::createTestFile() {
    
    char buf[1024 * 1024];
    string path = "/Users/youzhenghong/test/file";

    memset(buf, 0, sizeof(buf));

    long inc = 5;
    cout << " ready to create " << endl;
    for (int i = 1; i < 20;i++) {
        string fName = path + std::to_string(i) + ".txt";
        char *cstr = new char[fName.length() + 1];

        strcpy(cstr, fName.c_str());
        cout << "writing to " << cstr << ".....size: ";
        int fd = open(cstr, O_RDWR);
        if(fd < 0) {
            DieWithMessage("open fails");
        }
        double b = 0;
        for (int i = 0; i < inc; i++) {
            int cnt = 0;
            for (int j = 0; j < 512; j++) {
                if ((cnt=write(fd, buf, 1024 * 1024)) < 0) {
                  DieWithMessage("write error");
                }
                b += cnt;
            }
            
            
        }
        cout << b / (1024 * 1024 * 1024) << " GB" << endl;
        close(fd);
        inc++;
        
    }

} 

double NWTest::access(int fd) {
    double cnt = 0;
    char buf[4*1024*1024];;
    int rd = 0;
    while ((rd = read(fd, buf, sizeof(buf))) > 0) {

        cnt += rd;
    }
    
    return cnt;
}


void NWTest::testFileRead() {
    resetNWTest();
    
    
    
    
    char* result = "/Users/youzhenghong/test/result.txt";
    
    string path = "/Users/youzhenghong/test/file";
    int fds;

    double file_size = 0;
    for (int i = 1; i < 25; i++) {
        this->res.clear();
        system("sync & echo 1995927 | sudo -S purge");
        for (int j = 0; j < 30; j++) {
            string fName = path + std::to_string(i) + ".txt";
            char *cstr = new char[fName.length() + 1];
            strcpy(cstr, fName.c_str());
            fds = open(cstr, O_RDONLY);
            uint64_t start = this->timer.getCpuCycle();
            file_size = access(fds);
            uint64_t end = this->timer.getCpuCycle();
            close(fds);
            double msec = this->timer.cycleToMsSec(end - start);
            if (j > 2)
              this->res.push_back(msec);
        }

        cout << "file size:  " << file_size/(1024*1024*1024) << " GB" << "------" << "time stat:  min/stddev   " << getAvg() << " / " << getStddev()<<endl;    
        

        
    }
    
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
