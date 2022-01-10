#ifndef PROJECT_4_F21_APSAWICKI_MAIN_H
#define PROJECT_4_F21_APSAWICKI_MAIN_H

#include<iostream> // input/output
#include<unistd.h> // fork
#include<string> // strings
#include<csignal> // signals
#include<random> // random number gen
#include<ctime> // srand() with respect to time
#include<queue> // queue data structure for logs
#include<fstream> // write to log file
#include<pthread.h> // threads
#include<sys/times.h> // clock_gettime()
#include<sys/types.h> // thread ID
#include<vector> // vector data structure
#include<chrono> // 30 second timer
using namespace std;
using namespace std::chrono;

bool runCondition = true;
struct timespec stop, quit;
int setting;
time_point<system_clock> startTime, endTime;
int finalRec1, finalRec2;

struct SIGRECV {
    string sigType;
    long time;
    pthread_t ID;
};

struct sigRecvCompare{
    bool operator()(SIGRECV log1, SIGRECV log2){
        return log1.time > log2.time;
    }
};

priority_queue<SIGRECV, vector<SIGRECV>, sigRecvCompare> sigRecvLog; // declared in main.h so signalGen.cpp can add to queue, then I will write contents to file in main.cpp


pthread_mutex_t usr1Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t usr2Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t loggerLock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t filledQueue = PTHREAD_COND_INITIALIZER;
pthread_cond_t emptyQueue = PTHREAD_COND_INITIALIZER;


void blockSIGUSRs(){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
} // both signalGen and signalHandler need to block both SIGUSR signals so i put it in the header file

#endif //PROJECT_4_F21_APSAWICKI_MAIN_H
