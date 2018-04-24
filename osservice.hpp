#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <stdint>

class OsService{
  private:
    uint64_t _t_per_cycle;
    static uint64_t getTimeStamp();
    uint64_t convertCyclesToTime(uint64_t cycles);

  public:
    uint64_t testThreadContextSwitchTime();
    uint64_t testProcContextSwitchTime();
}

#endif
