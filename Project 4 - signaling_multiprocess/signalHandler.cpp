#include"main.h"

int usr1RecvCounter = 0;
int usr2RecvCounter = 0;

void unblockSIGUSR1(){
    sigset_t sigset;
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
}

void unblockSIGUSR2(){
    sigset_t sigset;
    sigaddset(&sigset, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
}


void signalHandler(int sig){
    printf("signal handler\n");

    // stop timer and get the log ready
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    SIGRECV log = {"", stop.tv_nsec, pthread_self()};

    if (sig == SIGUSR1){
         printf("handler received: SIGUSER1\n");

        log.sigType = "SIGUSR1";
        pthread_mutex_lock(&usr1Lock);
        usr1RecvCounter++;
        pthread_mutex_unlock(&usr1Lock);

    }
    else if (sig == SIGUSR2){
         printf("handler received: SIGUSER2\n");

        log.sigType = "SIGUSR2";
        pthread_mutex_lock(&usr2Lock);
        usr2RecvCounter++;
        pthread_mutex_unlock(&usr2Lock);

    }
    else{
        printf("Error: signal is something other than sigusr1 and sigusr2\n");
    }
     printf("Thread ID is: %lu\n\n", (long) pthread_self());


    // send the logs to the priority queue
    pthread_mutex_lock(&loggerLock);
    while(sigRecvLog.size() == 16 && runCondition){
        pthread_cond_signal(&filledQueue);
        pthread_cond_wait(&emptyQueue, &loggerLock); // wait for the logger thread to tell us that the queue is empty
    }

    sigRecvLog.push(log);

    pthread_mutex_unlock(&loggerLock);
}

void *receiverTypeOneFunc(void *arg){

    // need to block SIGUSR2 signals for this thread
    unblockSIGUSR1();
    signal(SIGUSR1, signalHandler);


    while(runCondition){
        sleep(1);
        printf("receiverTwo\n");
    }
    return nullptr;
}

void *receiverTypeTwoFunc(void *arg){

    // need to block SIGUSR1 signals for this thread
    unblockSIGUSR2();
    signal(SIGUSR2, signalHandler);

    while(runCondition){
        sleep(1);
        printf("receiverOne\n");
    }
    return nullptr;
}

long getDiffAvg(vector<SIGRECV> logs){

    long usrAvg = 0;
    int usrDiffCount = 0;


    // "fence-post problem" (i need the previous value to get the difference, so I need to start it off before I go into the loop)
    SIGRECV prevLog = logs.front();
    logs.erase(logs.begin());

    while (!logs.empty()){
        SIGRECV log = logs.front();
        logs.erase(logs.begin());

        usrDiffCount++;
        usrAvg += (log.time - prevLog.time);

        prevLog = log;
    }


    return (usrAvg/usrDiffCount);
}

void *loggerThreadFunc(void *arg){
    printf("LoggerThread is: %lu\n", (long) pthread_self());

    while (runCondition){
        printf("logger thread\n");
        vector<SIGRECV> signalusr1;
        vector<SIGRECV> signalusr2;


        pthread_mutex_lock(&loggerLock);
        while(sigRecvLog.size() < 16 && runCondition){ // i dont want the thread to become stuck if i no longer want the program to run
            pthread_cond_wait(&filledQueue, &loggerLock);
        }

        if (runCondition){
            ofstream outFile("sigRecLog.txt", ios_base::app);


            while(!sigRecvLog.empty()){
                SIGRECV log = sigRecvLog.top();
                sigRecvLog.pop();

                log.sigType == "SIGUSR1" ? signalusr1.push_back(log) : signalusr2.push_back(log);

                outFile << log.sigType << " received by thread [" << log.ID << "] at [" << log.time << "ns]\n";

            }

            outFile << "SIGUSR1 Avg Time Between Signals: " << getDiffAvg(signalusr1) << endl << "SIGUSR2 Avg Time Between Signals: " << getDiffAvg(signalusr2) << endl << endl;
            outFile.close();
        }

        pthread_cond_signal(&emptyQueue);
        pthread_mutex_unlock(&loggerLock);


    }
    pthread_cond_signal(&emptyQueue); // need to make sure my other threads won't get stuck in the pthread_wait()
    return nullptr;
}

void quitExecution(){
    if (setting){
                bool check = true;
                int count1 = 0;
                int count2 = 0;
                while(check){
                    sleep(1);

                    pthread_mutex_lock(&usr1Lock);
                    count1 = usr1RecvCounter;
                    pthread_mutex_unlock(&usr1Lock);

                    pthread_mutex_lock(&usr2Lock);
                    count2 = usr2RecvCounter;
                    pthread_mutex_unlock(&usr2Lock);
                    pthread_cond_signal(&emptyQueue);
                    pthread_cond_signal(&filledQueue);

                    printf("count: %i\n", (count1 + count2));
                    if (count1 + count2 > 100000){
                        check = false;

                        printf("CHECK IS FALSE\n");
                    }
                }
            }
            else{
                endTime = system_clock::now();
                duration<double> elapsed = endTime - startTime;
                while(elapsed.count() < 30){
                    endTime = system_clock::now();
                    elapsed = endTime - startTime;
        }
    }
    printf("should quit now\n");
    runCondition = false;
} // function that will stop programs when we hit either 30seconds of runtime or 100,000 signals

void spawnSignalHandlerThreads(){
    blockSIGUSRs();


    printf("starting signal handler\n");
    printf("Thread ID is: %lu\n\n", (long) pthread_self());

    pthread_t receiverTypeOne[2];
    for (int i = 0; i < 2; i++){
        if (pthread_create(&receiverTypeOne[i], nullptr, receiverTypeOneFunc, nullptr) != 0){
            printf("Error: failed to create receiverTypeOne thread\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("receiverThread1: %lu\n", receiverTypeOne[0]);
    printf("receiverThread1: %lu\n", receiverTypeOne[1]);

    pthread_t receiverTypeTwo[2];
    for (int i = 0; i < 2; i++){
        if (pthread_create(&receiverTypeTwo[i], nullptr, receiverTypeTwoFunc, nullptr) != 0){
            printf("Error: failed to create receiverTypeTwo thread\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("receiverThread2: %lu\n", receiverTypeTwo[0]);
    printf("receiverThread2: %lu\n", receiverTypeTwo[1]);


    pthread_t logger;
    if (pthread_create(&logger, nullptr, loggerThreadFunc, nullptr) != 0){
        printf("Error: failed to create logger thread\n");
        exit(EXIT_FAILURE);
    }



    quitExecution();

    // make sure to combine all threads before exiting

    pthread_cond_signal(&filledQueue); // make sure that logger thread isn't stuck
    pthread_cond_signal(&emptyQueue); // make sure receiver threads aren't stuck
    pthread_join(logger, nullptr);

    for (int i = 0; i < 2; i++){
        pthread_join(receiverTypeOne[i], nullptr);
    }
    for (int i = 0; i < 2; i++){
        pthread_join(receiverTypeTwo[i], nullptr);
    }

    printf("EXITED THREADS\n");
}
