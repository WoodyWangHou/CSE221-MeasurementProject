#include "osservice.h"
#include  <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint>
#include <vector>
#include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

OsService::OsService(){
  //constructor
  OsService::_t_per_cycle_ms = 0;
  uint64_t start_high;
  uint64_t start_low;

  uint64_t end_high;
  uint64_t end_low;

// record start time
  asm volatile (
         "CPUID\n\t"/*serialize*/
         "RDTSC\n\t"/*read the clock*/
         "mov %%edx, %0\n\t"
         "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)
         :: "%rax", "%rbx", "%rcx", "%rdx");

  sleep(SLEEP_TIME_SEC);

  asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (end_high), "=r" (end_low)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

  uint64_t start = (((uint64_t)start_high << 32) | start_low );
  uint64_t end = (((uint64_t)end_high << 32) | end_low );
  _t_per_cycle_ms = (end - start) / SLEEP_TIME_SEC * SEC_TO_MSEC;
}

/*****************************************************
* Private Members Implementation:
*****************************************************/

static uint64_t OsService::getCPUCycles(){
  uint64_t high;
  uint64_t low;

  asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (high), "=r" (low)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

  return (((uint64_t)high << 32) | low );
}

static double OsService::stddev(std::vector<double> input, double avg){
  double sd = 0.0;
  for(double i : input){
    sd += std::pow(i - avg, 2.0);
  }

  sd /= static_cast<double>(input.size());
  sd = std::pow(sd, 0.5);

  return sd;
}

/*****************************************************
* Public Members Implementation:
*****************************************************/


void OsService::testThreadContextSwitchTime(){

}

void OsService::testProcContextSwitchTime(uint64_t iter, double &dv, double &res){
  int pipes[2];
  pid_t pid;
  uint64_t end = 0; // used as buffer
  uint64_t start = 0;
  std::vector<double> times;
  uint64_t total_cycles = 0;

  for(uint64_t i = 0; i < iter; i++){
    if(pipe(pipes) == -1){
      std::cout << "pipes create failed" << "\n";
      exit(EXIT_FAILURE);
    }

    pid = fork();
    switch(pid){
      case -1:
      // fork fail
      std::cout << "fork process failed" << "\n";
      exit(EXIT_FAILURE);
      break;

      case 0:
      //child process
      end = OsService::getCPUCycles();
      write(pipes[1], &end, sizeof(end));
      exit(EXIT_SUCCESS);
      break;

      default:
      // parent process
      start = OsService::getCPUCycles();
      read(pipes[0], &end, sizeof(end)); // blocked until child process write its end cycle

      wait(NULL); // wait for child process to finish
      close(pipes[0]);
      close(pipes[1]);
    }

    uint64_t cur = end - start;
    times.push_back((double)cur / iter / (double)_t_per_cycle_ms); //accumulate cycles
    total_cycles += cur;
  }

  res = (double)(total_cycles / iter) / (double)_t_per_cycle_ms; // res in ms
  dv = OsService::stddev(times , res);
  return;
}
