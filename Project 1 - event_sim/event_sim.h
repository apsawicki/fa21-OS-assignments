#ifndef EVENT_SIM_H
#define EVENT_SIM_H

enum CONFIG {
    SEED,
    INIT_TIME,
    FIN_TIME,
    ARRIVE_MIN,
    ARRIVE_MAX,
    QUIT_PROB,
    NETWORK_PROB,
    CPU_MIN,
    CPU_MAX,
    DISK1_MIN,
    DISK1_MAX,
    DISK2_MIN,
    DISK2_MAX,
    NETWORK_MIN,
    NETWORK_MAX
}; // config settings

enum EVENT_TYPE {
    JOB_ARRIVAL=1,    // 1
    JOB_QUIT,         // 2
    CPU_ARRIVAL,      // 3
    CPU_FINISH,       // 4
    NET_ARRIVAL,      // 5
    NET_FINISH,       // 6
    DISK1_ARRIVAL,    // 7
    DISK1_FINISH,     // 8
    DISK2_ARRIVAL,    // 9
    DISK2_FINISH,     // 10
    SIMULATION_FINISH // 11
}; // all event types

struct EVENT{
    int jobID;
    int time;
    EVENT_TYPE type;
}; // what constitutes an event(what values each event will hold)

struct my_compare{ // How I learned to implement the struct into the priority queue https://www.tutorialspoint.com/stl-priority-queue-for-structure-or-class-in-cplusplus
    bool operator()(EVENT event1, EVENT event2){ // since my_compare is going to be used to compare values in the priority_queue, using operator() overloads the function
        return event1.time > event2.time;        // as i dont have my_compare taking in values when initializing the priority queue, so then it compares/sorts by time
    }
}; // compare function for the priority queue

float avg(float currentAvg, float newNum, float totalValues){
    return (((currentAvg * (totalValues - 1)) + newNum)/totalValues);
}

#endif // EVENT_SIM_H