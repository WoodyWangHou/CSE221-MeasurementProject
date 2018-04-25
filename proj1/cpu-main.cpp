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
#include "osservice.hpp"

int main(){
  const std::string DELIMITER = "*******************************";
  const uint64_t ITERATION = 100;

  OsService test;
  double proc_ctx_res;
  double proc_ctx_stdv;

  std::cout << DELIMITER << "\n";
  std::cout << "Process Context Switch Time Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  test.testProcContextSwitchTime(ITERATION, proc_ctx_stdv, proc_ctx_res);

  std::cout << "The measured average process context switch time: " + std::to_string(proc_ctx_res) + "ms" << '\n';
  std::cout << "The measured process context switch time standard deviation: " + std::to_string(proc_ctx_stdv) + "ms" << '\n';
  return 0;
}
