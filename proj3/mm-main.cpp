/******************************************************************************************
Project: UCSD CSE221 Course Project: CPU and OS Services Measurement

Author:
1. Hou Wang
2. Jinhao Tang
3. Zhenghong You

cpu-main.cpp:
main testing procedures
*******************************************************************************************/

#include <iostream>
#include <unistd.h>
#include <cstring>
#include "nwTest.hpp"

const std::string DELIMITER = "*******************************";

int main(char * arg){

  // MMTest mem_test;
  std::string args(arg);
  const uint64_t ITERATION = 5000;
  // make sure file is larger than memory
  const uint64_t NUM_PAGES = 3000000;
  const uint64_t PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

  std::cout << DELIMITER << "\n";
  std::cout << "Page Fault Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  return 0;
}
