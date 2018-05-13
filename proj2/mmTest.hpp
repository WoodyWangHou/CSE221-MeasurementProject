#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>
#include "timer.hpp"

class MMTest{
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
      test if a test need to be reset
    */
    bool isTestReset(){
      return res.empty();
    };

    /*
      reconstruct timer and result vector;
    */
    void resetMMTest(){
      res.clear();
      timer = Timer();
    };

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

    /*
      Page fault test
    */
    void testPageFault(uint64_t iter);
};

#endif
