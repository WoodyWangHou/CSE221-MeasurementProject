#ifndef OSSERVICE_H
#define OSSERVICE_H // define measurement functions for project 1 cpu parts

#include <stdint>

class OsService{
  private:
    uint64_t getTimeStamp();

  public:
    uint64_t testThreadContextSwitchTime();
    uint64_t testProcContextSwitchTime();
}

#endif
