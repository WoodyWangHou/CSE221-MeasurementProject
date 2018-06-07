#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>    /* for atoi() and exit() */
#include <string.h>    /* for memset() */
#include <unistd.h>    /* for close() */
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>       /* log2 */

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define PORT  8000

#define BLOCKSIZE 4096
#define BLOCKSIZE1 4096
#define NS 1000000
#define BEGIN 4
#define END 19
#define P 60


#include <iostream>
#include <fstream>
using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point getTime(){
    high_resolution_clock::time_point t = high_resolution_clock::now();
    return t;
}

double getTimeDif(const high_resolution_clock::time_point & t1, const high_resolution_clock::time_point & t2) {
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    return time_span.count();
}

double sequentialAccess(int fd, int &iter){
  double d = 0;
    iter = 0;
    char buffer[BLOCKSIZE];
    ssize_t ret_in = 1;
    auto t1 = getTime();
    auto t2 = getTime();
    while((ret_in ) > 0){
      t1 = getTime();
      ret_in  = read (fd, &buffer, BLOCKSIZE);
      t2 = getTime();
      d+= getTimeDif(t1,t2);
      iter++;
    }

    return d/ double(iter)*NS;
}

double randomAccess(int fd, int iter){
    char buffer[BLOCKSIZE];
    ssize_t ret_in;
    auto t1 = getTime();
    auto t2 = getTime();
    //cout << iter << endl;
    //cout<<"  dsfjlads " << iter<< endl;
    double result = 0;
    for (int i = 0; i < iter; i++) {
        int r = rand() % (iter*BLOCKSIZE);
        //lseek (fd,r*BLOCKSIZE, SEEK_SET);
        //cout << r << endl;
        t1 = getTime();
        lseek (fd,r, SEEK_SET);
        ret_in = read (fd, &buffer, BLOCKSIZE);
        t2 = getTime();
        result += getTimeDif(t1,t2);
    }
    return result/ double(iter)*NS;
}

long long readFile(char file[], int& fd) {
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror ("open error");
        return -1;
    }

    if (fcntl(fd, F_NOCACHE, 1) < 0) {
        cout << "Fail to disable cache" << endl;
        return -1;
    }

    if(-1 == fd)
    {
        printf("\n open() failed with error [%s]\n",strerror(errno));
        return -1;
    }
    struct stat buf;
    fstat(fd,&buf);
    return buf.st_size;
}

void measureReadFilePerformance(){
    for (int i = BEGIN; i <= END; i++) {
        string file ="/Users/tangjinhao/CSE221/CSE221-MeasurementProject/TestingData/RSRead/f";
        file += to_string(i) + ".txt";
        int n = file.length();
        char char_array[n+1];
        strcpy(char_array, file.c_str());
        int fd, iter;
        readFile(char_array, fd);
        cout << "Sequential Read Per Block "<<sequentialAccess(fd,iter) <<" us"<< endl;
        cout << "Random Read Per Block "<<randomAccess(fd,iter) <<" us"<< endl;
        cout << "File Size "<< log2 (iter * BLOCKSIZE) << endl;
    }
}

double contend(int i){
        //cout << i << endl;
        string file ="/Users/tangjinhao/CSE221/CSE221-MeasurementProject/TestingData/Contention/f";
        //string file ="/Users/tangjinhao/CSE221/CSE221-MeasurementProject/TestingData/RSRead/f";
        //file += to_string(i + 1) + ".txt";
        file +=  "1.txt";
        int n = file.length();
        char char_array[n+1];
        strcpy(char_array, file.c_str());

        int fd, iter;
        //iter = 10000;
        auto t1 = getTime();
        readFile(char_array, fd);

        sequentialAccess(fd,iter);
        auto t2 = getTime();
        return iter;
        //return double(getTimeDif(t1,t2)/ double(iter) * NS);
        //cout << getTimeDif(t1,t2)/ double(iter) * NS << endl;
        //cout << sequentialAccess(fd,iter) <<" us"<< endl;
        //cout << sequentialAccess(fd,iter) <<" us"<< endl;
        //cout << "Random Read Per Block "<<randomAccess(fd,iter) <<" us"<< endl;
}



void measureContention(int p){
    int pipes[p + 1];
    double v = -1111;
    double value = -1;
    if(pipe(pipes) == -1){
      std::cout << "pipes create failed" << "\n";
      exit(EXIT_FAILURE);
    }
    pid_t pid;
    std::cout << "Number of Processes: " << p << endl;
    auto t1 = getTime();
    for(uint64_t i = 0; i < p; i++){
        pid = fork();
        switch(pid){
            case -1:
                // fork fail
                std::cout << "fork process failed" << "\n";
                exit(1);
                break;

            case 0:
                v = contend(i);
                write(pipes[1], &v, sizeof(v));
                exit(1);
                break;

            default:
                // parent process
                read(pipes[0], &v, sizeof(v));
                //cout << v << endl;
                value += v;
                //cout << value << endl;
                wait(NULL); // wait for child process to finish
        }
    }
    auto t2 = getTime();
    //cout << value/double(p) << endl;
    cout << getTimeDif(t1,t2) / value * NS<< endl;
}

void measureContention(){
  for (int i = P -1; i < P; i+= 10) {
    measureContention(i);
  }
}


int main(int argc, char *argv[])
{
    //measureReadFilePerformance();
    measureContention();
    return 0;
}
