#ifndef OVERHEADS_H
#define OVERHEADS_H 

#include "timer.hpp"
#include <cstdint>
#include <vector>



class Overheads {
	private:
		Timer timer;
		void func0(){};
		void func1(int a1){};
		void func2(int a1, int a2){};
		void func3(int a1, int a2, int a3){};
		void func4(int a1, int a2, int a3, int a4){};
		void func5(int a1, int a2, int a3, int a4, int a5){};
		void func6(int a1, int a2, int a3, int a4, int a5, int a6){};
		void func7(int a1, int a2, int a3, int a4, int a5, int a6, int a7){};
	public:
		void testReadOverhead(int iter, double &dv, double &res);
		void testLoopOverhead(int iter, double &dv, double &res);
		void testProcedureCallOverhead0(int iter, double &dv, double &res);
		void testProcedureCallOverhead1(int iter, double &dv, double &res);
		void testProcedureCallOverhead2(int iter, double &dv, double &res);
		void testProcedureCallOverhead3(int iter, double &dv, double &res);
		void testProcedureCallOverhead4(int iter, double &dv, double &res);
		void testProcedureCallOverhead5(int iter, double &dv, double &res);
		void testProcedureCallOverhead6(int iter, double &dv, double &res);
		void testProcedureCallOverhead7(int iter, double &dv, double &res);
		void testSysCallOverhead(int iter, double &dv, double &res);
};

#endif
