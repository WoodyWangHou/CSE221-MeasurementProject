#include "timer.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



Timer::Timer(){
  //constructor
  Timer::_cycles_per_ms = 0;
  uint32_t start_high;
  uint32_t start_low;

  uint32_t end_high;
  uint32_t end_low;

// record start time
  asm volatile (
         "CPUID\n\t"/*serialize*/
         "RDTSC\n\t"/*read the clock*/
         "mov %%edx, %0\n\t"
         "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)
         :: "%eax", "%ebx", "%ecx", "%edx");

  sleep(SLEEP_TIME_SEC);

  asm volatile (
      "rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (end_high), "=r" (end_low)
		  :: "%eax", "%ebx", "%ecx", "%edx");

  uint64_t start = (((uint64_t)start_high << 32) | start_low );
  uint64_t end = (((uint64_t)end_high << 32) | end_low );
  _cycles_per_ms = (end - start) / SLEEP_TIME_SEC / SEC_TO_MSEC;
}

double Timer::cycleToSec(uint64_t cpu_cycle) {
	return (double)(cpu_cycle) / (double)_cycles_per_ms;
}
