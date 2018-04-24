#include "osservice.h"
#include  <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint>

OsService::OsService(){
  //constructor
  OsService::_t_per_cycle_ms = 0;
  uint64_t start_high;
  uint64_t start_low;

  uint64_t end_high;
  uint64_t end_low;

// record start time
  asm   volatile (
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

uint64_t OsService::convertCyclesToTime(uint64_t cycles){

}

uint64_t OsService::testThreadContextSwitchTime(){


}

uint64_t OsService::testProcContextSwitchTime(uint64_t iter){
  int pipes[2];
  pid_t pid;
  uint64_t end; // used as buffer
  uint64_t start;
  uint64_t total_cycles;

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

    total_cycles += end - start; //accumulate cycles
  }

  uint64_t avg_cycles = total_cycles / iter;




}
