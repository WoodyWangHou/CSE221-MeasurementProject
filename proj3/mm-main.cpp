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
#include "nwFsTest.hpp"

const double BYTE_TO_MBYTE = 1024.0 * 1024.0;

const std::string DELIMITER = "*******************************";
void logHeader(std::string msg){
  std::cout << DELIMITER << std::endl;
  std::cout << msg << std::endl;
  std::cout << DELIMITER << std::endl;
}

void log(std::string msg){
  std::cout << msg << std::endl;
}

int main(int argc, char* arg[]){

  NWTest test;
  std::string args;
  if(argc > 1){
    args = std::string((arg[1]));
  }

  const uint64_t REMOTE_ITERATION = 10;
  const uint64_t LOCAL_ITERATION = 100;
  std::string msg;
  // std::cout << args << std::endl;
  if(args.compare("-s") == 0){
    // server
    test.peakNetworkBandWidthServer();
    exit(1);
  }

  // test localhost network peak bandwidth
  logHeader("Peak Bandwidth Measurement: Localhost");

  test.peakNetworkBandWidthTestLocal(LOCAL_ITERATION);
  double MBytes = (test.getAvg() / BYTE_TO_MBYTE) * static_cast<double>(SEC_TO_MSEC);
  double stdv = (test.getStddev() / BYTE_TO_MBYTE) * static_cast<double>(SEC_TO_MSEC);
  msg = "Measured Peak BandWidth Remote Average Throughput: " + std::to_string(MBytes) + " MByte/sec";
  log(msg);

  msg = "Measured Peak BandWidth Remote Throughput Standard Deviation: " + std::to_string(stdv) + " MByte/sec";
  log(msg);

  // test remote network peak bandwidth
  logHeader("Peak Bandwidth Measurement: Remote (AWS)");

  test.peakNetworkBandWidthTestRemote(REMOTE_ITERATION);
  MBytes = (test.getAvg() / BYTE_TO_MBYTE) * static_cast<double>(SEC_TO_MSEC);
  stdv = (test.getStddev() / BYTE_TO_MBYTE) * static_cast<double>(SEC_TO_MSEC);
  msg = "Measured Peak BandWidth Remote Average Throughput: " + std::to_string(MBytes) + " MByte/sec";
  log(msg);

  msg = "Measured Peak BandWidth Remote Throughput Standard Deviation: " + std::to_string(stdv) + " MByte/sec";
  log(msg);

  return 0;
}
