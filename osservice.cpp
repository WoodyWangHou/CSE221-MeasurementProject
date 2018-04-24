#include "osservice.h"
#include  <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint>

OsService::OsService(){
  //constructor
  OsService::_t_per_cycle = 0;
  uint64_t start_high;
  uint64_t start_low;

  uint64_t end_high;
  uint64_t end_low;

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
  uint64_t end_buffer;
  uint64_t start;

  if(pipe(pipes) == -1){
    cout << "pipes create failed" << "\n";
    exit(EXIT_FAILURE);
  }

  for(uint64_t i = 0; i < iter; i++){
    pid = fork();
    switch(pid){
      case -1:
      // fork fail
      cout << "fork process failed" << "\n";
      exit(EXIT_FAILURE);
      break;

      case 0:
      //child process
      buffer = OsService::getCPUCycles();
      write(pipes[1], &end_buffer, sizeof(end_buffer));
      exit(EXIT_SUCCESS);
      break;

      default:
      // parent process
      start = OsService::getCPUCycles();
      read(pipes[0], &end_buffer, sizeof(end_buffer));
    }


  }


}
