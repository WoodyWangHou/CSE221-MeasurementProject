#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>
#include <cstring>
#include <ctime>
#include "timer.hpp"

const unsigned short BW_TEST_PORT = 8429;

const uint8_t LISTEN = 1;
const uint8_t SEND_LOCAL = 2;
const uint8_t SEND_REMOTE = 3;

const uint64_t MAXPENDING = 5;
const std::string REMOTEIP = "52.25.77.52"; // to be filled in, aws(may change)
const std::string LOCALIP = "127.0.0.1";
const uint64_t BUFSIZE = 1024 * 1024; // 1Mb, based on Imbench
const uint64_t TOTALSIZE = 50; // 50 mb

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

	public:
    /**
      Paras: 1). input value in a vector. Please ensure the element has been converted to time already
      2). average of this vector of elements.
      return:
    **/

    double getStddev();

    /*
      get average value stored in res.
    */

    double getAvg();

		/**
		* peak bandwidth test
		*/
		void peakNetworkBandWidthServer();
		void peakNetworkBandWidthTestRemote(uint64_t iter);
		void peakNetworkBandWidthTestLocal(uint64_t iter);
};

#endif
