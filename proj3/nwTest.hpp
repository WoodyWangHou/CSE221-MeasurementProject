#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>
#include <cstring>
#include "timer.hpp"

const int BW_TEST_PORT = 8888;
const uint8_t LISTEN = 1;
const uint8_t SEND = 2;
const uint64_t MAXPENDING = 5; 

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
			  std::cerr << msg << std::endl;
			  exit(1);
		}

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
		void peakBandwidthTest(uint64_t iter, std::string args);

};

#endif
