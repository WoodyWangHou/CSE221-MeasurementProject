#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <stdint>
#include <vector>

const uint64_t SLEEP_TIME_SEC = 1; // sleep for 1 second
const uint64_t SEC_TO_MSEC = 1000;

class OsService{
  private:
    uint64_t _t_per_cycle_ms;
    static uint64_t getTimeStamp();
    static double stddev(std::vector<uint64_t> input);
    static uint64_t convertCyclesToTime(uint64_t cycles);

  public:
    uint64_t testThreadContextSwitchTime();
    uint64_t testProcContextSwitchTime();
}

#endif
