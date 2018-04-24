#include "osservice.h"
#include <linux/kernel.h>
#include <pthread.h>
#include <stdint>

static void
uint64_t OsService::getTimeStamp(){
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

uint64_t OsService::testThreadContextSwitchTime(){

}

uint64_t OsService::testProcContextSwitchTime(){

}
