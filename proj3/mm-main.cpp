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
#include "nwTest.hpp"

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
  double MBytes;
  double stdv;
  NWTest test;
  std::string args;
  if(argc > 1){
    args = std::string((arg[1]));
  }

  const uint64_t REMOTE_ITERATION = 10;
  const uint64_t LOCAL_ITERATION = 10;
  std::string msg;
  // std::cout << args << std::endl;
  if(args.compare("-s") == 0){
    // server
    test.peakNetworkBandWidthServer();
    exit(1);
  }

  // ping server
  if(args.compare("-p") == 0){
    // server
    test.peakNetworkBandWidthServer();
    exit(1);
  }



  // test localhost network peak bandwidth
  logHeader("Peak Bandwidth Measurement: Localhost");

  test.peakNetworkBandWidthTestLocal(LOCAL_ITERATION);
  MBytes = test.getAvg();
  stdv = test.getStddev();
  msg = "Measured Peak BandWidth Remote Average Throughput: " + std::to_string(MBytes) + " MByte/sec";
  log(msg);

  msg = "Measured Peak BandWidth Remote Throughput Standard Deviation: " + std::to_string(stdv) + " MByte/sec";
  log(msg);

  // // test remote network peak bandwidth
  logHeader("Peak Bandwidth Measurement: Remote (AWS)");

  test.peakNetworkBandWidthTestRemote(REMOTE_ITERATION);
  MBytes = test.getAvg();
  stdv = test.getStddev();
  msg = "Measured Peak BandWidth Remote Average Throughput: " + std::to_string(MBytes) + " MByte/sec";
  log(msg);

  msg = "Measured Peak BandWidth Remote Throughput Standard Deviation: " + std::to_string(stdv) + " MByte/sec";
  log(msg);

  // Sequential File Read Time
  for(uint64_t filesize = 1; filesize <= 128; filesize  *= 2){

      logHeader("File Read Sequential Access Time: " + std::to_string(filesize) + " Mbytes");

      test.fileSequentialReadTest(LOCAL_ITERATION, filesize);
      double tm = test.getAvg(); // local average file read time, msec/Mbytes
      stdv = test.getStddev();
      msg = "Measured File Read Sequential Access Time: " + std::to_string(tm) + " ms";
      log(msg);

      msg = "Measured File Read Sequential Access Standard Deviation: " + std::to_string(stdv) + " ms";
      log(msg);

      // Random File Read Time
      logHeader("File Read Random Access Time: " + std::to_string(filesize) + " Mbytes");

      test.fileRandomReadTest(LOCAL_ITERATION, filesize);
      tm = test.getAvg(); // local average file read time, msec/Mbytes
      stdv = test.getStddev();
      msg = "Measured File Read Random Access Time: " + std::to_string(tm) + " ms";
      log(msg);

      msg = "Measured File Read Random Access Standard Deviation: " + std::to_string(stdv) + " ms";
      log(msg);
  }



  
  return 0;
}
