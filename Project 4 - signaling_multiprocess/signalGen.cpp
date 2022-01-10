#include"main.h"

void handleTermination(int sig){

    if (sig == SIGTERM){
        runCondition = false;
    }
    else{
        printf("NOT CORRECT SIGNAL\n");
    }
}

void unblockSIGTERM(){
    sigset_t sigset;
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
}

void signalGen(pid_t pid[]) { // pid[0] will be the process with the receiver threads
    printf("signal generator\n");
    srand(getpid()); // seed in regards to the pid so each process will have something different and it will be pseudo-random each time
    blockSIGUSRs();
    unblockSIGTERM();

    signal(SIGTERM, handleTermination);



    while (runCondition) {
        // sleep random interval from (0.01 to 0.1) seconds - (10000 and 100000) micro seconds
         usleep(((rand() % 90000) + 10001));

        // generates SIGUSR1 OR SIGUSR2 depending on random number generator being 1 or 0
        string signal;
        if (rand() % 2){
            signal = "SIGUSR1";
            kill(pid[0], SIGUSR1);

        }
        else{
            signal = "SIGUSR2";
            kill(pid[0], SIGUSR2);

        }
        // printf("signal generated\n");

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
        ofstream outFile("sigGenLog.txt", ios_base::app);
        outFile << signal << " : [" << stop.tv_nsec << "]\n";
        outFile.close();


    }
    // printf("gen leaving\n");
    exit(EXIT_SUCCESS);
}