#include "event_sim.h"
#include <iostream>
#include <cstdio>
#include <queue>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


int getConfig(int arr[], const string& file){ // file handling + getting config values from external file

    ifstream inFile(file);
    if (!inFile.is_open()){
        printf("CANNOT OPEN FILE\n");
        return 1;
    }
    string line;
    int i = 0;

    while (getline(inFile, line)){
        int space = (int) line.find(' ');
        int value = stoi(line.substr(space, line.length()));
        arr[i] = value;
        i++;
    }

    return 0;
}


int jobArrival(EVENT currJob, priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& CPUstatus){
    EVENT newJob{((currJob.jobID) + 1), (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), JOB_ARRIVAL};
    EVENT_HANDLER.push(newJob);


    EVENT cpuArrival{currJob.jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), CPU_ARRIVAL};


    if (CPU_queue.empty() && (CPUstatus == 0)){
        EVENT_HANDLER.push(cpuArrival);
    }

    CPU_queue.push(cpuArrival);
  return 0;
}


int cpuArrival(priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& CPUstatus, float& CPU_utilization){

    if ((CPUstatus == 1) || CPU_queue.empty()){ // if the CPU is currently full or there is nothing in the queue then it cant work
        return 1;
    }

    EVENT cpuFinish{(CPU_queue.front().jobID), (curTime + (config[CPU_MIN] + 1 + (rand() % (config[CPU_MAX] - config[CPU_MIN])))), CPU_FINISH};
    EVENT_HANDLER.push(cpuFinish); // add the CPU_FINISH event with the jobID of the recently popped job from CPU_queue
    CPU_queue.pop();

    // handling for stats.txt utilization part
    CPU_utilization += (float) (cpuFinish.time - curTime);

    CPUstatus = 1;
    return 0;
}


int cpuFinish(EVENT currJob, priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& CPUstatus, queue<EVENT>& DISK1_queue, queue<EVENT>& DISK2_queue, queue<EVENT>& NET_queue, int& NETstatus, int& DISK1status, int& DISK2status){
    int eventChoice = (1 + (rand() % 100)); // random number from 1 to 100

    if (eventChoice <= config[QUIT_PROB]){ // QUIT
        EVENT jobQuit{currJob.jobID, curTime, JOB_QUIT};
        EVENT_HANDLER.push(jobQuit);
    }
    else if (eventChoice > config[QUIT_PROB] && eventChoice <= (config[QUIT_PROB] + config[NETWORK_PROB])){ // NETWORK

        EVENT netArrival{currJob.jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), NET_ARRIVAL};
        if (NET_queue.empty() && !NETstatus){
            EVENT_HANDLER.push(netArrival);
        }
        NET_queue.push(netArrival);
    }
    else{ // DISK
        EVENT disk1Arrival{currJob.jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), DISK1_ARRIVAL};
        EVENT disk2Arrival{currJob.jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), DISK2_ARRIVAL};

        if (DISK1_queue.size() == DISK2_queue.size()){
            if ((rand() % 2)){ // either 0 or 1

                if (DISK1_queue.empty() && !DISK1status){
                    EVENT_HANDLER.push(disk1Arrival);
                }
                DISK1_queue.push(disk1Arrival); // adding to disk1 if true
            }
            else{

                if (DISK2_queue.empty() && !DISK2status){
                    EVENT_HANDLER.push(disk2Arrival);
                }
                DISK2_queue.push(disk2Arrival); // adding to disk2 if false
            }
        }
        else{
            if (DISK1_queue.size() < DISK2_queue.size()){ // DISK 1

                if (DISK1_queue.empty() && !DISK1status){
                    EVENT_HANDLER.push(disk1Arrival);
                }
                DISK1_queue.push(disk1Arrival); // adding to disk1 if true

            }
            else{ // DISK 2

                if (DISK1_queue.empty() && !DISK2status){
                    EVENT_HANDLER.push(disk2Arrival);
                }
                DISK2_queue.push(disk2Arrival); // adding to disk2 if false
            }


        }
    }

    CPUstatus = 0;

    if (!CPU_queue.empty()){ // will give the cpu a job if the cpu_queue is not empty
        EVENT cpuArrival{CPU_queue.front().jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), CPU_ARRIVAL};
        EVENT_HANDLER.push(cpuArrival);
    }


    return 0;
}


int netArrival(priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& NET_queue, const int config[], int curTime, int& NETstatus, float& NET_utilization){
    if (NETstatus || NET_queue.empty()){ // if the CPU is currently full then dont use it
        return 1;
    }

    EVENT netFinish{(NET_queue.front().jobID), (curTime + (config[NETWORK_MIN] + 1 + (rand() % (config[NETWORK_MAX] - config[NETWORK_MIN])))), NET_FINISH};
    EVENT_HANDLER.push(netFinish); // add the CPU_FINISH event with the jobID of the recently popped job from CPU_queue
    NET_queue.pop();

    // handling for stats.txt utilization part
    NET_utilization += (float) (netFinish.time - curTime);

    NETstatus = 1;
    return 0;
}


int netFinish(EVENT currJob, priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& NETstatus, queue<EVENT>& NET_queue){

    CPU_queue.push(currJob);

    if (!NET_queue.empty()){
        EVENT netArrival{NET_queue.front().jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), NET_ARRIVAL};
        EVENT_HANDLER.push(netArrival);
    }

    NETstatus = 0;
    return 0;
}


int disk1Arrival(priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& DISK1_queue, const int config[], int curTime, int& DISK1status, float& DISK1_utilization){
    if (DISK1status || DISK1_queue.empty()){
        return 1;
    }

    EVENT disk1Finish{(DISK1_queue.front().jobID), (curTime + (config[DISK1_MIN] + 1 + (rand() % (config[DISK1_MAX] - config[DISK1_MIN])))), DISK1_FINISH};
    EVENT_HANDLER.push(disk1Finish);
    DISK1_queue.pop();

    // handling for stats.txt utilization part
    DISK1_utilization += (float) (disk1Finish.time - curTime);

    DISK1status = 1;
    return 0;
}


int disk1Finish(EVENT currJob, priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& DISK1status, queue<EVENT>& DISK1_queue){

    CPU_queue.push(currJob);

    if (!DISK1_queue.empty()){
        EVENT disk1Arrival{DISK1_queue.front().jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), NET_ARRIVAL};
        EVENT_HANDLER.push(disk1Arrival);
    }

    DISK1status = 0;
    return 0;
}


int disk2Arrival(priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& DISK2_queue, const int config[], int curTime, int& DISK2status, float& DISK2_utilization){
    if (DISK2status || DISK2_queue.empty()){
        return 1;
    }

    EVENT disk2Finish{(DISK2_queue.front().jobID), (curTime + (config[DISK2_MIN] + 1 + (rand() % (config[DISK2_MAX] - config[DISK2_MIN])))), DISK2_FINISH};
    EVENT_HANDLER.push(disk2Finish);
    DISK2_queue.pop();

    // handling for stats.txt utilization part
    DISK2_utilization += (float) (disk2Finish.time - curTime);

    DISK2status = 1;
    return 0;
}


int disk2Finish(EVENT currJob, priority_queue<EVENT, vector<EVENT>, my_compare>& EVENT_HANDLER, queue<EVENT>& CPU_queue, const int config[], int curTime, int& DISK2status, queue<EVENT>& DISK2_queue){

    CPU_queue.push(currJob);

    if (!DISK2_queue.empty()){
        EVENT disk2Arrival{DISK2_queue.front().jobID, (curTime + (config[ARRIVE_MIN] + 1 + (rand() % (config[ARRIVE_MAX] - config[ARRIVE_MIN])))), NET_ARRIVAL};
        EVENT_HANDLER.push(disk2Arrival);
    }

    DISK2status = 0;
    return 0;
}


int findAvgMaxSize(float& max_CPU_size, float& max_NET_size, float& max_DISK1_size, float& max_DISK2_size, float& cpuSize, float& netSize, float& disk1Size, float& disk2Size,
                   queue<EVENT>& CPU_queue, queue<EVENT>& NET_queue, queue<EVENT>& DISK1_queue, queue<EVENT>& DISK2_queue, float& cpuChanges,
                   float& netChanges, float& disk1Changes, float& disk2Changes, float& avg_CPU_queue_size, float& avg_NET_queue_size,
                   float& avg_DISK1_queue_size, float& avg_DISK2_queue_size){

    cpuChanges++;
    if (max_CPU_size < (float) CPU_queue.size()){
        max_CPU_size = (float) CPU_queue.size();
    }
    avg_CPU_queue_size = avg(avg_CPU_queue_size, (float) CPU_queue.size(), cpuChanges);

    netChanges++;
    if (max_NET_size < (float) NET_queue.size()){
        max_NET_size = (float) NET_queue.size();
    }
    avg_NET_queue_size = avg(avg_NET_queue_size, (float) NET_queue.size(), netChanges);

    disk1Changes++;
    if (max_DISK1_size < (float) DISK1_queue.size()){
        max_DISK1_size = (float) DISK1_queue.size();
    }
    avg_DISK1_queue_size = avg(avg_DISK1_queue_size, (float) DISK1_queue.size(), disk1Changes);

    disk2Changes++;
    if (max_DISK2_size < (float) DISK2_queue.size()){
        max_DISK2_size = (float) DISK2_queue.size();
    }
    avg_DISK2_queue_size = avg(avg_DISK2_queue_size, (float) DISK2_queue.size(), disk2Changes);

    return 0;
}


int main(int argc, char **argv){

    if (argc == 1){
        printf("You must provide one config file: ./[name] [config file]");
        return 1;
    }
    else if (argc > 2){
        printf("You must only provide one config file: ./[name] [config file]");
        return 1;
    }

    // writing logs.txt file
    ofstream logsOutFile("logs.txt" /*"/mnt/c/Users/apsaw/CLionProjects/project-1-fall21-apsawicki/logs.txt"*/);
    if (!logsOutFile.is_open()){
        printf("ERROR OPENING FILE");
    }

    // writing stats.txt file
    ofstream statsOutFile("stats.txt" /*"/mnt/c/Users/apsaw/CLionProjects/project-1-fall21-apsawicki/stats.txt"*/);
    if (!statsOutFile.is_open()){
        printf("ERROR OPENING FILE");
    }

    // getting the config values
    int config[15];
    getConfig(config, argv[1]/*"/mnt/c/Users/apsaw/CLionProjects/project-1-fall21-apsawicki/config.txt"*/);

    // seeding my random number generator
    srand(config[SEED]);

    // making queues
    queue <EVENT> CPU_queue, NET_queue, DISK1_queue, DISK2_queue;
    priority_queue <EVENT, vector<EVENT>, my_compare> EVENT_HANDLER;

    // create the start/finish events + add to priority queue
    EVENT startSimulation{1, config[INIT_TIME], JOB_ARRIVAL};
    EVENT endSimulation{0, config[FIN_TIME], SIMULATION_FINISH};
    EVENT_HANDLER.push(startSimulation);
    EVENT_HANDLER.push(endSimulation);

    // status to see whether the device is in use or not and initializing curTime var
    int CPUstatus = 0, NETstatus = 0, DISK1status = 0, DISK2status = 0, curTime;

    // vars to track queue size
    float max_CPU_size = 0, max_NET_size = 0, max_DISK1_size = 0, max_DISK2_size = 0;
    float avg_CPU_queue_size = 0, avg_NET_queue_size = 0, avg_DISK1_queue_size = 0, avg_DISK2_queue_size = 0;

    // vars to track device utilization
    float CPU_utilization = 0, NET_utilization = 0, DISK1_utilization = 0, DISK2_utilization = 0;

    // vars to track device job completion amount
    float CPU_jobs = 0,  NET_jobs = 0, DISK1_jobs = 0, DISK2_jobs = 0;

    // will need the number of changes if im going to take the avg
    float cpuChanges = 0, netChanges = 0, disk1Changes = 0, disk2Changes = 0;


    while (!EVENT_HANDLER.empty() && (EVENT_HANDLER.top().type != 11)){


        EVENT currJob = EVENT_HANDLER.top();
        EVENT_HANDLER.pop();
        curTime = currJob.time;
        logsOutFile << "time: " << currJob.time << ", job: " << currJob.jobID << ", event: ";

        // recording size before event so I can compare if they had their sizes changed
        float cpuSize = (float) CPU_queue.size(), netSize = (float) NET_queue.size(), disk1Size = (float) DISK1_queue.size(), disk2Size = (float) DISK2_queue.size();

        switch(currJob.type){
            case JOB_ARRIVAL:{
                jobArrival(currJob, EVENT_HANDLER, CPU_queue, config, curTime, CPUstatus);
                logsOutFile << "JOB_ARRIVAL";

                // CPU--
                break;
            }
            case JOB_QUIT:{
                // nothing needs to happen since the job will be lost if it isn't put anywhere
                logsOutFile << "JOB_QUIT";
                break;
            }
            case CPU_ARRIVAL:{
                cpuArrival(EVENT_HANDLER, CPU_queue, config, curTime, CPUstatus, CPU_utilization);
                logsOutFile << "CPU_ARRIVAL";
                break;
            }
            case CPU_FINISH:{
                cpuFinish(currJob, EVENT_HANDLER, CPU_queue, config, curTime, CPUstatus, DISK1_queue, DISK2_queue, NET_queue, NETstatus, DISK1status, DISK2status);
                logsOutFile << "CPU_FINISH";
                CPU_jobs++;
                break;
            }
            case NET_ARRIVAL:{
                netArrival(EVENT_HANDLER, NET_queue, config, curTime, NETstatus, NET_utilization);
                logsOutFile << "NET_ARRIVAL";
                break;
            }
            case NET_FINISH:{
                netFinish(currJob, EVENT_HANDLER, CPU_queue, config, curTime, NETstatus, NET_queue);
                logsOutFile << "NET_FINISH";
                NET_jobs++;
                break;
            }
            case DISK1_ARRIVAL:{
                disk1Arrival(EVENT_HANDLER, DISK1_queue, config, curTime, DISK1status, DISK1_utilization);
                logsOutFile << "DISK1_ARRIVAL";
                break;
            }
            case DISK1_FINISH:{
                disk1Finish(currJob, EVENT_HANDLER, CPU_queue, config, curTime, DISK1status, DISK1_queue);
                logsOutFile << "DISK1_FINISH";
                DISK1_jobs++;
                break;
            }
            case DISK2_ARRIVAL:{
                disk2Arrival(EVENT_HANDLER, DISK2_queue, config, curTime, DISK2status, DISK2_utilization);
                logsOutFile << "DISK2_ARRIVAL";
                break;
            }
            case DISK2_FINISH:{
                disk2Finish(currJob, EVENT_HANDLER, CPU_queue, config, curTime, DISK2status, DISK2_queue);
                logsOutFile << "DISK2_FINISH";
                DISK2_jobs++;
                break;
            }
            case SIMULATION_FINISH:{
                logsOutFile << "SIMULATION_FINISH"; // it should never get here
                break;
            }
            default:{
                printf("SWITCH STATEMENT ERROR");
            }
        }
        findAvgMaxSize(max_CPU_size, max_NET_size, max_DISK1_size, max_DISK2_size, cpuSize, netSize, disk1Size, disk2Size, CPU_queue, NET_queue,
                       DISK1_queue, DISK2_queue, cpuChanges, netChanges, disk1Changes, disk2Changes, avg_CPU_queue_size, avg_NET_queue_size,
                       avg_DISK1_queue_size, avg_DISK2_queue_size);


        logsOutFile << endl;

    }


    // writing stats to stats.txt
    statsOutFile << "Average & Maximum Size per Queue(Avg, Max): \n" << "CPU: " << avg_CPU_queue_size << ", " << max_CPU_size << endl
                 << "NETWORK: " << avg_NET_queue_size << ", " << max_NET_size << endl << "DISK1: " << avg_DISK1_queue_size
                 << ", " << max_DISK1_size << endl << "DISK2: " << avg_DISK2_queue_size << ", " << max_DISK2_size << endl << endl;

    statsOutFile << "Utilization(out of 1): \n" << "CPU: " << CPU_utilization/((float) (config[FIN_TIME] - config[INIT_TIME])) << endl << "NETWORK: "
                 << NET_utilization/((float) (config[FIN_TIME] - config[INIT_TIME])) << endl << "DISK1: "
                 << DISK1_utilization/((float) (config[FIN_TIME] - config[INIT_TIME])) << endl << "DISK2: "
                 << DISK2_utilization/((float) (config[FIN_TIME] - config[INIT_TIME])) << endl << endl;

    statsOutFile << "Throughput (per " << (((float) config[FIN_TIME])/10.0) << " units of time)" << endl << "CPU: " << (CPU_jobs/10.0) << endl
                 << "NETWORK: " << (NET_jobs/10.0) << endl << "DISK1: " << (DISK1_jobs/10.0) << endl << "DISK2: " << (DISK2_jobs/10.0) << endl;

    logsOutFile.close();
    statsOutFile.close();

    return 0;
}