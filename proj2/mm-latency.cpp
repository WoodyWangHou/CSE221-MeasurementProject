#include <iostream>
using namespace std;

using namespace std::chrono;

#define LOOPS 10000000

#define LOOPS 10000000
#define TOP 26
#define START 8
#define G 1000000000

high_resolution_clock::time_point getTime(){
  high_resolution_clock::time_point t = high_resolution_clock::now();
  return t;
}

double getTimeDif(const high_resolution_clock::time_point & t1, const high_resolution_clock::time_point & t2) {
   duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
   return time_span.count();
}

void memlatency(){
  int size = 1;
      srand(time(NULL));
      long stride = (1<<10) - 1;
      //vector<int> stridelist={4, 64, 256, 8192, 524288, 4194304, 16777216, 33554432};//33554432
      //vector<int> stridelist={4, 64, 256, 8192, 524288, 4194304, 16777216, 33554432};

      vector<int> stridelist;//={64, 128,256, 1024, 1048576, 4194304, 16777216, 33554432};//33554432
      for (int i = 2; i < 25; i++) {
       stridelist.push_back(1<<i);
      }

      size = size << START;
      int value = 0;
      long index=0;

      for (int i = START; i < TOP; i++) {
              double MAX = 0;
              for (int x = 0; x < stridelist.size(); x++) {
                  stride = stridelist[x] - 1;
              size = 1 << i;
              int* array = (int*)malloc(size);
              // initialize the value
              long arraysize = 1 << (i - 2);
              for (int index = 0; index < arraysize; index++) {
                  array[index] = (index + stride) % arraysize;
              }

              auto t1 = getTime();
              for (long index = 0; index < LOOPS; index++) {
                  value = array[value];
              }
              auto t2 = getTime();
              double d = getTimeDif(t1,t2);
                  MAX = max(MAX, d);
          free(array);
      }
          cout <<i << " " << MAX/ LOOPS * G << endl;
      }

}
