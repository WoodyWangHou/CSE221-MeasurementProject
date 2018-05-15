/******************************************************************************************
Project: UCSD CSE221 Course Project: CPU and OS Services Measurement

Author:
1. Hou Wang
2. Jinhao Tang
3. Zhenghong You

osservice.cpp:
implementation of testing procedures: context switching time measurement
*******************************************************************************************/

#include "osservice.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <cstdint>
#include <vector>
#include <math.h>

/*****************************************************
* Constructor Implementation:
*****************************************************/

OsService::OsService(){
  //constructor
  OsService::_cycles_per_ms = 0;
  uint32_t start_high;
  uint32_t start_low;

  uint32_t end_high;
  uint32_t end_low;

// record start time
  asm volatile (
         "CPUID\n\t"/*serialize*/
         "RDTSC\n\t"/*read the clock*/
         "mov %%edx, %0\n\t"
         "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)
         :: "%eax", "%ebx", "%ecx", "%edx");

  sleep(SLEEP_TIME_SEC);

  asm volatile (
      "rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (end_high), "=r" (end_low)
		  :: "%eax", "%ebx", "%ecx", "%edx");

  uint64_t start = (((uint64_t)start_high << 32) | start_low );
  uint64_t end = (((uint64_t)end_high << 32) | end_low );
  _cycles_per_ms = (end - start) / SLEEP_TIME_SEC / SEC_TO_MSEC;
}

/*****************************************************
* Private Members Implementation:
*****************************************************/

uint64_t OsService::getCPUCycles(){
  uint32_t high;
  uint32_t low;

  asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (high), "=r" (low)
		  :: "%eax", "%ebx", "%ecx", "%edx");

  return (((uint64_t)high << 32) | low );
}

double OsService::stddev(std::vector<double> input, double avg){
  double sd = 0.0;
  for(double i : input){
    sd += pow(i - avg, 2.0);
  }

  sd /= static_cast<double>(input.size());
  sd = pow(sd, 0.5);

  return sd;
}

/*****************************************************
* Public Members Implementation:
*****************************************************/

void* test(void *end){	
	pthread_exit(NULL);	
}

void *testThreads(void *end){
  uint32_t high;
  uint32_t low;

  asm volatile ("rdtscp\n\t"
      "mov %%edx, %0\n\t"
      "mov %%eax, %1\n\t"
      "cpuid\n\t"
      : "=r" (high), "=r" (low)
      :: "%eax", "%ebx", "%ecx", "%edx");
  uint64_t *res = (uint64_t *)end;
  *res = (((uint64_t)high << 32) | low );

  pthread_exit(NULL);
}

void OsService::testThreadCreation(uint64_t iter, double &dv, double &res){
  pthread_t td;
  uint64_t start, end;
  uint64_t total_cycles = 0;
  std::vector<double> times;
  uint64_t count = 0;
  
  for (int i = 0; i < iter; i++) {
    start = OsService::getCPUCycles();
    //pthread_create(&td, NULL, &test,&end);
    pthread_create(&td, NULL, &testThreads, &end);
    end = OsService::getCPUCycles();
    pthread_join(td, NULL);
    total_cycles += end - start;
    double cur = end - start;
    times.push_back(( (double)cur / (double)_cycles_per_ms));
    count++;
  }
  res = (double)(total_cycles / count) / (double)_cycles_per_ms;
  std::cout << "Thread Creation       " << std::endl;
  std::cout << std::to_string(total_cycles / count) << std::endl;
  dv = OsService::stddev(times , res);
  return;
}

void OsService::testProcessCreation(uint64_t iter, double &dv, double &res){
   pid_t pid;
  uint64_t end = 0; // used as buffer
  uint64_t start = 0;
  uint64_t total_cycles = 0;
  std::vector<double> times;
  uint64_t count = 0;
  
  for(uint64_t i = 0; i < iter; i++){
    start = OsService::getCPUCycles();
    pid = fork();

    switch(pid){
      case -1:
      // fork fail
      std::cout << "fork process failed" << "\n";
      exit(1);
      break;

      case 0:
      //child process
      exit(1);
      break;

      default:
      // parent process
      end = OsService::getCPUCycles();
      total_cycles += end - start;
      double cur = end - start;
      times.push_back(( (double)cur / (double)_cycles_per_ms));
      count++;
      wait(NULL); // wait for child process to finish
    }
  }
  res = (double)(total_cycles / count) / (double)_cycles_per_ms;
  dv = OsService::stddev(times , res);
  return;
}




void OsService::testThreadContextSwitchTime(uint64_t iter, double &dv, double &res){
  pthread_t tid;
  uint64_t start = 0;
  uint64_t end = 0;
  std::vector<double> times;
  uint64_t total_cycles = 0;
  uint64_t count = 0;

  // for(uint64_t i = 0; i < iter; i++){
  //   start = OsService::getCPUCycles();
  //   if(pthread_create(&tid, NULL, &testThreads, &end) != 0){
  //     std::cout << "threads create failed" << "\n";
  //     exit(EXIT_FAILURE);
  //   }else{
  //     sched_yield();
  //   }

  //   pthread_join(tid, NULL);

  //   if(end > start){
  //     uint64_t cur = end - start;
  //     times.push_back(((double)cur / (double)_cycles_per_ms)); //accumulate cycles
  //     total_cycles += cur;
  //     count++;
  //   }
  // }


    for(uint64_t i = 0; i < iter; i++){

    if(pthread_create(&tid, NULL, &testThreads, &end) != 0){
      std::cout << "threads create failed" << "\n";
      exit(EXIT_FAILURE);
    }else{
      start = OsService::getCPUCycles();
    }
    pthread_join(tid, NULL);

    uint64_t cur = 0;
    if(end > start){
       cur = end - start;
    } else {
    	cur = start - end;
    }

    times.push_back(((double)cur / (double)_cycles_per_ms)); //accumulate cycles
      total_cycles += cur;
      count++;
  }

  res = ((double)(total_cycles / count) / (double)_cycles_per_ms); // res in ms
  dv = OsService::stddev(times , res);
  return;
}

void OsService::testProcContextSwitchTime(uint64_t iter, double &dv, double &res){
  int pipes[2];
  if(pipe(pipes) == -1){
    std::cout << "pipes create failed" << "\n";
    exit(EXIT_FAILURE);
  }

  pid_t pid;
  uint64_t end = 0; // used as buffer
  uint64_t start = 0;
  std::vector<double> times;
  uint64_t total_cycles = 0;
  uint64_t count = 0; //count of valid measure

  for(uint64_t i = 0; i < iter; i++){
    
    pid = fork();

    switch(pid){
      case -1:
      // fork fail
      std::cout << "fork process failed" << "\n";
      exit(EXIT_FAILURE);
      break;

      case 0:
      //child process
      end = OsService::getCPUCycles();
      write(pipes[1], &end, sizeof(end));
      exit(EXIT_SUCCESS);
      break;

      default:
      // parent process
      start = OsService::getCPUCycles();
      read(pipes[0], &end, sizeof(end)); // blocked until child process write its end cycle

      wait(NULL); // wait for child process to finish
    }

    if(end > start){
      uint64_t cur = end - start;
      times.push_back(((double)cur / (double)_cycles_per_ms)); //accumulate cycles
      total_cycles += cur;
      count++;
    }
  }

  close(pipes[0]);
  close(pipes[1]);

  res = ((double)(total_cycles / count) / (double)_cycles_per_ms); // res in ms
  dv = OsService::stddev(times , res);
  return;
}
