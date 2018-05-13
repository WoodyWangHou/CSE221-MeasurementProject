#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <math.h>

/*
	utility for timing, some code is copied from OsServiceClass, Acknowledgments to Wang Hou
	5/13/2018: Reused code from Zhenghong.
*/
using namespace std;
const uint64_t SLEEP_TIME_SEC = 1; // sleep for 1 second
const uint64_t SEC_TO_MSEC = 1000;

class Timer {
	private:
		/**
			This is the number of CPU cycles in 1 ms
			You can count CPU cycles for executing an instruction, then divide it by this value

			example:
			time = (double)(total_cycles) / (double)_t_per_cycle_ms;
		**/
		uint64_t _cycles_per_ms;

	public:
		Timer();
		double cycleToMsSec(uint64_t cpu_cycle);

		/**
			return: current cpu time in cpu cycles.
		**/
		inline uint64_t getCpuCycle() {
			uint32_t cycles_low, cycles_high;
			uint64_t cycle;
			asm volatile ("CPUID\n\t"
						"RDTSC\n\t"
		 				"mov %%edx, %0\n\t"
		 				"mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
						"%rax", "%rbx", "%rcx", "%rdx");

			cycle = ( ((uint64_t)cycles_high << 32) | cycles_low );
			//cout << "cycle:  " << cycle << endl;
			return cycle;
		}
		inline void warmUp() {
			this->getCpuCycle();
			this->getCpuCycle();
		}

};

#endif
