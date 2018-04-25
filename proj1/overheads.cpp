#include "overheads.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;


void Overheads::testReadOverhead(int iter, double &dv, double &res) {
	// measure overhead of reading operation
	Timer myTimer;
	myTimer.warmUp();

	unsigned long flags;
	uint64_t start = 0;
	uint64_t end = 0;
	vector<double> vec_time;
	double acc_time = 0;
	uint64_t cnt = 0;
	vec_time.clear();

	for (int i = 0; i < iter; i++) {

		// disable interrupt
		//preempt_disable();
		//raw_local_irq_save(flags);

		start = myTimer.getCpuCycle();
		myTimer.getCpuCycle();
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += cur;
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);

}







void Overheads::testProcedureCallOverhead(int iter, double &dv, double &res) {

}
void Overheads::testSysCallOverhead(int iter, double &dv, double &res) {

}