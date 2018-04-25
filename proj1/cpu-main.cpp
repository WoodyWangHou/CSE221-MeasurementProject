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
#include "overheads.hpp"
int main(){
  // declare common output format
  const std::string DELIMITER = "*******************************";
  const uint64_t ITERATION = 1000;

  OsService test;
  double proc_ctx_res;
  double proc_ctx_stdv;

  Overheads overHeadTest;
  double read_op_res, loop_op_res, func_call_res[8];
  double read_op_stdv, loop_op_std, func_call_std[8];






  test.testProcContextSwitchTime(ITERATION, proc_ctx_stdv, proc_ctx_res);

  // read and loop overhead test
  overHeadTest.testReadOverhead(ITERATION, read_op_stdv, read_op_res);
  overHeadTest.testLoopOverhead(ITERATION, loop_op_std, loop_op_res);

  // procedure call overhead test
  overHeadTest.testProcedureCallOverhead0(ITERATION, func_call_std[0], func_call_res[0]);
  overHeadTest.testProcedureCallOverhead1(ITERATION, func_call_std[1], func_call_res[1]);
  overHeadTest.testProcedureCallOverhead2(ITERATION, func_call_std[2], func_call_res[2]);
  overHeadTest.testProcedureCallOverhead3(ITERATION, func_call_std[3], func_call_res[3]);
  overHeadTest.testProcedureCallOverhead4(ITERATION, func_call_std[4], func_call_res[4]);
  overHeadTest.testProcedureCallOverhead5(ITERATION, func_call_std[5], func_call_res[5]);
  overHeadTest.testProcedureCallOverhead6(ITERATION, func_call_std[6], func_call_res[6]);
  overHeadTest.testProcedureCallOverhead7(ITERATION, func_call_std[7], func_call_res[7]);


  std::cout << DELIMITER << "\n";
  std::cout << "Process Context Switch Time Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  std::cout << "Iterations: " + std::to_string(ITERATION) << '\n';
  std::cout << "The measured average process context switch time: " + std::to_string(proc_ctx_res) + "ms" << '\n';
  std::cout << "The measured process context switch time standard deviation: " + std::to_string(proc_ctx_stdv) + "ms" << '\n';

  test.testThreadContextSwitchTime(ITERATION, proc_ctx_stdv, proc_ctx_res);

  std::cout << DELIMITER << "\n";
  std::cout << "Threads Context Switch Time Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  std::cout << "Iterations: " + std::to_string(ITERATION) << '\n';
  std::cout << "The measured average process context switch time: " + std::to_string(proc_ctx_res) + "ms" << '\n';
  std::cout << "The measured process context switch time standard deviation: " + std::to_string(proc_ctx_stdv) + "ms" << '\n';

  // Read Operation Overhead Measurement Result;
  std::cout << DELIMITER << "\n";
  std::cout << "Read Operation Overhead Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  std::cout << "Iterations: " + std::to_string(ITERATION) << '\n';
  std::cout << "The measured average read operation overhead: " + std::to_string(read_op_res) + "ms" << '\n';
  std::cout << "The measured standard deviation of read operation overhead: " + std::to_string(read_op_stdv) + "ms" << '\n';


  // Loop Operation Overhead Measurement Result;
  std::cout << DELIMITER << "\n";
  std::cout << "Loop Operation Overhead Measurement" <<"\n";
  std::cout << DELIMITER << "\n";

  std::cout << "Iterations: " + std::to_string(ITERATION) << '\n';
  std::cout << "The measured average Loop operation overhead: " + std::to_string(loop_op_res) + "ms" << '\n';
  std::cout << "The measured standard deviation of loop operation overhead: " + std::to_string(loop_op_std) + "ms" << '\n';


  for (int i = 0; i < 8; i++) {
      std::cout << DELIMITER << "\n";
      std::cout << "Procedure call Operation Overhead Measurement, " << std::to_string(i) << " parameters." <<"\n";
      std::cout << DELIMITER << "\n";

      std::cout << "Iterations: " + std::to_string(ITERATION) << '\n';
      std::cout << "The measured average procedure call Operation overhead: " + std::to_string(func_call_res[i]) + "ms" << '\n';
      std::cout << "The measured standard deviation of procedure call Operation overhead: " + std::to_string(func_call_std[i]) + "ms" << '\n';
  }







  return 0;
}
