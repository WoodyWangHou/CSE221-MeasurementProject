#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <cstdint>
#include <vector>

const uint64_t SLEEP_TIME_SEC = 1; // sleep for 1 second
const uint64_t SEC_TO_MSEC = 1000;

class OsService{
  private:
    uint64_t _t_per_cycle_ms;
    uint64_t getCPUCycles();
    double stddev(std::vector<double> input, double avg);

  public:
    OsService();
    void testThreadContextSwitchTime(uint64_t iter, double &dv, double &res);
    void testProcContextSwitchTime(uint64_t iter, double &dv, double &res);
};

#endif
