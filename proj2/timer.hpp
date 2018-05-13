#ifndef TIMER_H
#define TIMER_H

#include "osservice.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include <math.h>

/*
	utility for timing, some code is copied from OsServiceClass, Acknowledgments to Wang Hou
*/
using namespace std;

class Timer {
	private:
		uint64_t _cycles_per_ms;

	public:
		Timer();
		double cycleToSec(uint64_t cpu_cycle);
		double stddv (std::vector<double> input, double avg);

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
