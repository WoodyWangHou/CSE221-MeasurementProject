/******************************************************************************************
Project: UCSD CSE221 Course Project: CPU and OS Services Measurement

Author:
1. Hou Wang
2. Jinhao Tang
3. Zhenghong You

osservice.cpp:
implementation of testing procedures: context switching time measurement
*******************************************************************************************/

#include "mmTest.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mann.h>
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
  uint64_t PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
  std::cerr << "Page size is: " << PAGE_SIZE << std::endl;
  const uint64_t TEST_LENGTH = 1000;

  int fd = 0;
  char* data;
  uint64_t start = 0;
  uint64_t end = 0;
  double p_time = 0.0;
  // test if test has been reset, otherwise reset it
  if(!isTestReset()){
    resetMMTest();
  }

  fd = open("./pageFaultTestFile", O_CREATE | O_RDWR, S_IRUSR | S_IWUSR);
  if(fd < 0){
    std::cerr << errno << std::endl;
    exit(1);
  }

  data = static_cast<char *>(mmap(NULL, TEST_LENGTH * PAGE_SIZE,
                              PROT_WRITE | PROT_READ,
                              MAP_SHARED, fd, 0));
  this->timer.warmUp();

  for(unsigned i = 0; i < TEST_LENGTH; ++i){
    start = this->timer.getCpuCycle();
    data[i * PAGE_SIZE] = 'm';
    end = this->timer.getCpuCycle();
    p_time = this->cycleToMsSec(end - start);
  }
  if(munmap(data, TEST_LENGTH * PAGE_SIZE) < 0){
    std::cerr << errno << std::endl;
  }
  close(fd);
}
