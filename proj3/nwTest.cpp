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
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <resolv.h>
#include <netdb.h>


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
        DieWithMessage("connect() failed");
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
* Ping Test Implementation:
**************************************************************/
struct packet
{
  struct icmphdr hdr;
  char msg[PACKETSIZE-sizeof(struct icmphdr)];
};



int NWTest::createPingSocket() {

    int sock;
    int opt = 1;

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) 
      DieWithMessage("creat ping server socket error");

    if (setsockopt(servSock, SOL_SOCKET, 
      SO_REUSEPORT, &opt, sizeof(opt)))
    DieWithError("setsockopt() failed");

    if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
        DieWithError("Request nonblocking I/O");

    return sock;
}



void NWTest::pingServer() {

    int servSock;  /* socket descriptor for server */
    int clntSock;  /* socket descriptor for client */
    int opt = 1;  /* opt for setsocketopt */
    
    unsigned int clntLen;            /* Length of client address data structure */


    servSock = createPingSocket();
   
    struct sockaddr_in srcAddr;     /* Source address */
    unsigned char buf[1024];        /* msg buffer */



    for (;;){ 
      int len=sizeof(addr);
      int bytes = 0;

      bzero(buf, sizeof(buf));
      bytes = recvfrom(sd, buf, sizeof(buf), MSG_NOBLOCK, (struct sockaddr*)&srcAddr, &len);
      if ( bytes > 0 )
        sendToSrc(buf, bytes, srcAddr);
      else
        perror("recvfrom");
    }
    exit(0);
}


struct packet NWTest::icmpPacket() {
    struct packet pckt;
    
    bzero(&pckt, sizeof(pckt));
    pckt.hdr.type = ICMP_ECHO;
    memset(pckt.msg, 0, sizeof(pckt.msg));
    pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
    return pckt;
}


void NWTest::senToSrc(unsigned char buf[], int sz, struct sockaddr_in src) {
    cout << "Received icmp message" << sz << " bytes...." <<"ready to reply back" << endl;
}


void NWTest::ping(struct sockaddr_in addr, int sock, strcut packet pckt) {

    struct sockaddr_in r_addr;

    for (;;) { 
        int len=sizeof(r_addr);

        if (recvfrom(sock, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0 ) {
            cout << sizeof(pckt) << "bytes from " << inet_ntoa(&r_addr->sin_addr) << ": ";
            break;
        } 
        if (sendto(sock, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0)
          DieWithMessage("sendto");     
    }
}

void NWTest::p() {
    cout << " test " << endl;
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

    //
    const int val=255;
    int cnt=1;
    struct packet pckt;
    int sock;
    struct sockaddr_in s_addr;


    sock = createPingSocket();
    pckt = icmpPacket();
    src_addr = srcAddr(LOCALIP);
    
    

    this->timer.warmUp();
    for (int i = 0; i < iter; i++) {
        uint64_t start = this.getCpuCycle();
        ping(src_addr, sock, pckt);
        uint64_t end = this.getCpuCycle();
        double msec = this->timer.cycleToMsSec(end - start);
        cout << "time= "<< msec << " ms" << endl;
        this.res.push_back(msec);
    }
    cout << "---- ping "<< inet_ntoa(&src_addr->sin_addr) << " stat ----" << endl;
    cout << "round trip avg/stddev:  " << res.getAvg() << " / " << res.getStddev() << endl;

}

unsigned short NWTest::checksum(void *b, int len){ 
    unsigned short *buf = b;
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
