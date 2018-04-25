#ifndef OVERHEADS_H
#define OVERHEADS_H 

#include "timer.hpp"
#include <cstdint>
#include <vector>



class Overheads {
	private:
		Timer timer;
	public:
		void testReadOverhead(int iter, double &dv, double &res);
		void testLoopOverhead(int iter, double &dv, double &res);
		void testProcedureCallOverhead(int iter, double &dv, double &res);
		void testSysCallOverhead(int iter, double &dv, double &res);
};

#endif
