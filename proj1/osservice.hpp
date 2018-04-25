#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>

const uint64_t SLEEP_TIME_SEC = 1; // sleep for 1 second
const uint64_t SEC_TO_MSEC = 1000;

class OsService{
  private:
    /**
      This is the number of CPU cycles in 1 ms
      You can count CPU cycles for executing an instruction, then divide it by this value

      example:
      time = (double)(total_cycles) / (double)_t_per_cycle_ms;
    **/
    uint64_t _cycles_per_ms;

    /**
      return: current cpu time in cpu cycles.
    **/
    uint64_t getCPUCycles();

    /**
      Paras: 1). input value in a vector. Please ensure the element has been converted to time already
      2). average of this vector of elements.
      return:
    **/
    double stddev(std::vector<double> input, double avg);

  public:
    OsService();
    void testThreadContextSwitchTime(uint64_t iter, double &dv, double &res);
    void testProcContextSwitchTime(uint64_t iter, double &dv, double &res);
};

#endif
