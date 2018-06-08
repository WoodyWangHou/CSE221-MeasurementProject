#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>
#include <cstring>
#include <ctime>
#include "timer.hpp"
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>


const uint64_t PACKET_SIZE = 64;
const unsigned short BW_TEST_PORT = 8429;
const unsigned short PING_TEST_CLNT_PORT = 5001;
const unsigned short PING_TEST_SERVER_PORT = 4003;

const std::string REMOTEIP = "52.25.77.52"; // to be filled in, aws(may change)
const std::string LOCALIP = "127.0.0.1"; // local host
// char* pingLocal = "ping -c 10 127.0.0.1";
// char* pingRemote = "ping -c 10 52.25.77.52";
const uint8_t LISTEN = 1;
const uint8_t SEND_LOCAL = 2;
const uint8_t SEND_REMOTE = 3;
const uint8_t PING = 6;

const uint64_t MAXPENDING = 5;
const uint64_t BUFSIZE = 1024 * 1024; // 1Mb, based on Imbench
const uint64_t TOTALSIZE = 50; // 50 mb
const uint64_t PING_SZ = 64;



const std::string TEST_FILENAME_BASE = "./test.t"; // test file name, will be created if not exist, 8Mb large
const uint64_t BYTE_TO_MBYTE = 1024 * 1024;
const uint64_t FILE_BUFSIZE = 64 * 1024; // 64K buffer

class NWTest{
  private:
    /*
      Instantiate a timer class for recording time
    */
    Timer timer;

    /*
      create a vector to store series of test results
    */
    std::vector<double> res;

    /*
      reconstruct timer and result vector;
    */
    void resetNWTest(){
      res.clear();
      timer = Timer();
    };

		/*
		* kill process with message
		*/

		void DieWithMessage(std::string message){
			  std::cerr << message << std::endl;
			  exit(1);
		}

    /*
    * logger
    */

    void log(std::string msg){
      std::cerr << msg << std::endl;
      return;
    }

		/*
		*	socket setup helpers
		*/
		int createSocket(unsigned short servPort);
		void bindSocket(int servSock, unsigned short servPort);
		void HandleTCPClient(int clntSock);
		void connectSocket(int clntSock, std::string ip, unsigned short port);
		void bandWidthMeasurement(int servSock, uint64_t iter);
		int setUpSocket(unsigned short servPort, int mode);

    int createPingSocket();
    void sendToSrc(unsigned char buf[], int sz, struct sockaddr_in src);
    unsigned short checksum(void *b, int len);
    void ping(struct sockaddr_in addr, int sock,int mode);
    struct sockaddr_in srcAddr(string ip);
    /*
    * file measurement helpers
    */

    int openFileWithNoCache(std::string fileName, uint64_t fileSize);
    void sequentialReadMeasurement(int fd,  uint64_t iter, uint64_t fileSize);
    void randomReadMeasurement(int fd,  uint64_t iter, uint64_t fileSize);
    std::string exec(const char* cmd);
    double access(int fd);

    void handleEcho(int sock);

	public:
    /**
      Paras: 1). input value in a vector. Please ensure the element has been converted to time already
      2). average of this vector of elements.
      return:
    **/
    double testSetup;
    double testTearDown;

    std::pair<double, double> setUpTearLocal();
    std::pair<double, double> setUpTearRemote();

    double getStddev();

    /*
      get average value stored in res.
    */

    double getAvg();

		/**
		* peak bandwidth test, 3.2
		*/
		void peakNetworkBandWidthServer();
		void peakNetworkBandWidthTestRemote(uint64_t iter);
		void peakNetworkBandWidthTestLocal(uint64_t iter);

    /**
    * file read test: 4.2, 4.3
    */
    void fileSequentialReadTest(uint64_t iter, uint64_t fileSize);
    void fileRandomReadTest(uint64_t iter, uint64_t fileSize);

    /*
     * test Round trip time. 
     */
    void p(char* cmd);
    void p();
    void pingTestServer();
    void testPingLocal(uint64_t iter);
    void testPingRemote(uint64_t inter);
    void testFileRead();
    void createTestFile();


};

#endif
