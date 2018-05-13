/******************************************************************************************
Project: UCSD CSE221 Course Project: CPU and OS Services Measurement

Author:
1. Hou Wang
2. Jinhao Tang
3. Zhenghong You

osservice.cpp:
implementation of testing procedures: context switching time measurement
*******************************************************************************************/

#include "osservice.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <cstdint>
#include <vector>
#include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

/*****************************************************
* Private Members Implementation:
*****************************************************


/*****************************************************
* Public Members Implementation:
*****************************************************/


double MMTest::getStddev() {
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

double MMTest::getAvg(){
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

void MMTest::testPageFault(uint64_t iter){
  // test if test has been reset, otherwise reset it
  if(!isTestReset()){
    resetMMTest();
  }


}
