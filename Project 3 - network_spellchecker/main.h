#ifndef PROJ_3_NETWORKED_SPELL_CHECKER_APSAWICKI_MAIN_H
#define PROJ_3_NETWORKED_SPELL_CHECKER_APSAWICKI_MAIN_H
#include <string>
#include <chrono>

struct CONNECTION {
    int clientFD;
    std::chrono::high_resolution_clock::time_point requestTime; // chrono (the way I am measuring time) needs this data type for the time to be stored
    int priority;
};

struct my_compare{
    bool operator()(CONNECTION connection1, CONNECTION connection2){
        return connection1.priority < connection2.priority;
    }
};

pthread_mutex_t connectionMTX = PTHREAD_MUTEX_INITIALIZER; // lock to make a critical section on connection queue
pthread_cond_t emptySpace = PTHREAD_COND_INITIALIZER; // will let the producer know when it is safe to produce
pthread_cond_t nonEmptyQueue = PTHREAD_COND_INITIALIZER; // will let the consumers know when it is safe to consume

pthread_mutex_t loggerMTX = PTHREAD_MUTEX_INITIALIZER;  // lock to make a critical section on logger queue
pthread_cond_t nonEmptyLogQueue = PTHREAD_COND_INITIALIZER; // condition var to let the logger thread know when the queue is NOT empty

#endif //PROJ_3_NETWORKED_SPELL_CHECKER_APSAWICKI_MAIN_H
