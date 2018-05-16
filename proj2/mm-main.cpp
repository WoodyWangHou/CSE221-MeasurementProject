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
#include "mmTest.hpp"

const std::string DELIMITER = "*******************************";

int main(){
  // declare common output format
  MMTest mem_test;
  const uint64_t ITERATION = 5000;
  const uint64_t PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

  std::cout << DELIMITER << "\n";
  std::cout << "Page Fault Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  mem_test.testPageFault(ITERATION);
  std::cout << "Average Page Fault time: " << mem_test.getAvg() << " ms" << std::endl;
  std::cout << "Standard Deviation of page fault time: " << mem_test.getStddev() << std::endl;
  std::cout << "Average byte access time via page: " << mem_test.getAvg() / (double) PAGE_SIZE << " ms" << std::endl;

  return 0;
}
