#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>
#include "timer.hpp"
#define K 10
#define SZ 1000000
#define ITER 10000

struct Node {
	int a[K];
	Node() {
		for(int i = 0; i < K; i++) {
			a[i] = 0;
		}
	}
};

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
    void writeToMem(Node &n) {
    	for (int i = 0; i < K; i++) {
    		n.a[i] = 1;
    	}
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

    /*
      Page fault test
    */
    void testPageFault(uint64_t iter);

    /*
		memory read bandwidth test
    */
    void testReadBandwidth(uint64_t iter);
    void testWriteBandwidth(uint64_t iter);


};

#endif
