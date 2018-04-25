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

	//unsigned long flags;
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



void Overheads::testLoopOverhead(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		for (int j = 0; j < iter; j++);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur / iter);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}


void Overheads::testProcedureCallOverhead0(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func0();
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead1(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func1(1);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead2(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func2(1,2);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead3(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func3(1, 2, 3);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead4(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func4(1, 2, 3, 4);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead5(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func5(1, 2, 3, 4, 5);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}

void Overheads::testProcedureCallOverhead6(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func6(1, 2, 3, 4, 5, 6);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}
void Overheads::testProcedureCallOverhead7(int iter, double &dv, double &res) {
	Timer myTimer;
	myTimer.warmUp();

	//unsigned long flags;
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
		this->func7(1, 2, 3, 4, 5, 6, 7);
		end = myTimer.getCpuCycle();

		// enable interrupt
		//raw_local_irq_restore(flags);
		//preempt_enable();

		if (end > start) {
			double cur = myTimer.cycleToSec(end - start);
			vec_time.push_back(cur);
			acc_time += vec_time.back();
			cnt++;
		}
	}
	
	res = acc_time / cnt;
	dv = myTimer.stddv(vec_time, res);
}




void Overheads::testSysCallOverhead(int iter, double &dv, double &res) {

}