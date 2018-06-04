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
#include <sys/mman.h>
#include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

/*****************************************************
* Private Members Implementation:
*****************************************************/


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
    const uint64_t PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    int fd = 0;
    char* data;
    uint64_t start = 0;
    uint64_t end = 0;
    double p_time = 0.0;
    // test if test has been reset, otherwise reset it
    if(!isTestReset()){
        resetMMTest();
    }

    fd = open("./pageFaultTestFile", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd < 0){
        std::cerr << errno << std::endl;
        exit(1);
    }

    ftruncate(fd, iter * PAGE_SIZE);
    data = static_cast<char *>(mmap(NULL, iter * PAGE_SIZE,
                                PROT_WRITE | PROT_READ | PROT_EXEC,
                                MAP_SHARED, fd, 0));

    if(data == MAP_FAILED){
        std::cerr << "map fail" << std::endl;
        std::cerr << errno << std::endl;
        exit(1);
    }

    this->timer.warmUp();
    for(unsigned i = 0; i < iter; ++i){
        start = this->timer.getCpuCycle();
        data[i * PAGE_SIZE] = 'm';
        end = this->timer.getCpuCycle();
        p_time = this->timer.cycleToMsSec(end - start);
        this->res.push_back(p_time);
    }
    if(munmap(data, iter * PAGE_SIZE) < 0){
        std::cerr << errno << std::endl;
    }

    close(fd);
}



/*************************************************************
* Test memory read and write bandwidth Implementation:
**************************************************************/


void MMTest::testReadBandwidth(uint64_t iter) {
	uint64_t start = 0;
  	uint64_t end = 0;
  	double p_time = 0.0;

  	this->timer.warmUp();
  	this->res.clear();
  	
  	
  	std::cout << "stride size is: " << sizeof(int) << " bytes" << std::endl;
  	long array[SZ];
  	long n;
  	for (int i = 0; i < SZ; i++) {
  		array[i] = 0;
  	}
  	for (int j = 0; j < 1000; j++) {
  		start = this->timer.getCpuCycle();
	  	for(int i = 0; i < SZ; i+=50){
	  		// 5 unrolling
		    n = array[i];
		    n = array[i + 1];
		    n = array[i + 2];
		    n = array[i + 3];
		    n = array[i + 4];
		    n = array[i + 5];
		    n = array[i + 6];
		    n = array[i + 7];
		    n = array[i + 8];
		    n = array[i + 9];
		    n = array[i + 10];
		    n = array[i + 11];
		    n = array[i + 12];
		    n = array[i + 13];
		    n = array[i + 14];
		    n = array[i + 15];
		    n = array[i + 16];
		    n = array[i + 17];
		    n = array[i + 18];
		    n = array[i + 19];
		    n = array[i + 20];
		    n = array[i + 21];
		    n = array[i + 22];
		    n = array[i + 23];
		    n = array[i + 24];
		    n = array[i + 25];
		    n = array[i + 26];
		    n = array[i + 27];
		    n = array[i + 28];
		    n = array[i + 29];
		    n = array[i + 30];
		    n = array[i + 31];
		    n = array[i + 32];
		    n = array[i + 33];
		    n = array[i + 34];
		    n = array[i + 35];
		    n = array[i + 36];
		    n = array[i + 37];
		    n = array[i + 38];
		    n = array[i + 39];
		    n = array[i + 40];
		    n = array[i + 41];
		    n = array[i + 42];
		    n = array[i + 43];
		    n = array[i + 44];
		    n = array[i + 45];
		    n = array[i + 46];
		    n = array[i + 47];
		    n = array[i + 48];
		    n = array[i + 49];
		}
		end = this->timer.getCpuCycle();
		p_time = this->timer.cycleToMsSec(end - start);
		this->res.push_back((sizeof(long) * SZ * 1.0/1000000)/p_time);
  	}
}


void MMTest::testWriteBandwidth(uint64_t iter) {
	uint64_t start = 0;
  	uint64_t end = 0;
  	double p_time = 0.0;

  	this->timer.warmUp();
  	this->res.clear();
  	
  	long carray[SZ] __attribute__((aligned (64)));
  	//int n;
  	memset(carray, 0, SZ * sizeof(long));

  	std::cout << "stride size is: " << sizeof(long) << " bytes" << std::endl;

  	for (int j = 0; j < 1000; j++) {
  		start = this->timer.getCpuCycle();
	  	for(int i = 0; i < SZ; i+=50){
	  		// 5 unrolling
		    carray[i] = 1;
		    carray[i + 1] = 1;
		    carray[i + 2] = 1;
		    carray[i + 3] = 1;
		    carray[i + 4] = 1;
		    carray[i + 5] = 1;
		    carray[i + 6] = 1;
		    carray[i + 7] = 1;
		    carray[i + 8] = 1;
		    carray[i + 9] = 1;
		    carray[i + 10] = 1;
		    carray[i + 11] = 1;
		    carray[i + 12] = 1;
		    carray[i + 13] = 1;
		    carray[i + 14] = 1;
		    carray[i + 15] = 1;
		    carray[i + 16] = 1;
		    carray[i + 17] = 1;
		    carray[i + 18] = 1;
		    carray[i + 19] = 1;
		    carray[i + 20] = 1;
		    carray[i + 21] = 1;
		    carray[i + 22] = 1;
		    carray[i + 23] = 1;
		    carray[i + 24] = 1;
		    carray[i + 25] = 1;
		    carray[i + 26] = 1;
		    carray[i + 27] = 1;
		    carray[i + 28] = 1;
		    carray[i + 29] = 1;
		    carray[i + 30] = 1;
		    carray[i + 31] = 1;
		    carray[i + 32] = 1;
		    carray[i + 33] = 1;
		    carray[i + 34] = 1;
		    carray[i + 35] = 1;
		    carray[i + 36] = 1;
		    carray[i + 37] = 1;
		    carray[i + 38] = 1;
		    carray[i + 39] = 1;
		    carray[i + 40] = 1;
		    carray[i + 41] = 1;
		    carray[i + 42] = 1;
		    carray[i + 43] = 1;
		    carray[i + 44] = 1;
		    carray[i + 45] = 1;
		    carray[i + 46] = 1;
		    carray[i + 47] = 1;
		    carray[i + 48] = 1;
		    carray[i + 49] = 1;
		}
		end = this->timer.getCpuCycle();
		p_time = this->timer.cycleToMsSec(end - start);
		this->res.push_back((sizeof(long) * SZ * 1.0/1000000)/p_time);
  	}
}
