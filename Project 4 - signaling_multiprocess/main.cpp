#include <cstring>
#include"signalGen.cpp"
#include"signalHandler.cpp"

using namespace std;
using namespace std::chrono;

pid_t pid[3];
pid_t mainPid = pthread_self();


void blockSIGTERM(){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}


int main(int argc, char** argv){
    ofstream genOutFile("sigGenLog.txt"); // overwrite the previous log file
    genOutFile << "";

    ofstream recvOutFile("sigRecLog.txt"); // overwrite the previous log file
    recvOutFile << "";

    startTime = system_clock::now();

    signal(SIGTERM, handleTermination);

    if(argc != 2){
        printf("Incorrect Command Usage: [./main] [0 or 1]\n0: 30 seconds\n1: 100,000 signals\n");
        exit(EXIT_FAILURE);
    }
    else{
        setting = stoi(string(argv[1]));
    }
//    setting = 1;

    // creation of signal handler process
    if ((pid[0] = fork()) == 0){
        spawnSignalHandlerThreads();
    }
    else if (pid[0] < 0){
        printf("fork() failure - sign handler\n");
        exit(EXIT_FAILURE);
    }

    // creation of signal generator processes
    if (runCondition){ // if statement so the process returning from
        for (int i = 1; i < 3; i++){
            if((pid[i] = fork()) == 0){
                signalGen(pid);
            }
            else if (pid[i] < 0){
                printf("fork() failure - sig gen\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (runCondition){
        signalGen(pid); // the parent becomes a signal gen when it is done creating processes
    }


    blockSIGTERM(); // block SIGTERM for this process so we can send SIGTERM to the other processes

    // kill remaining processes - the process that should be here right now is the pid[1] process
    kill(pid[1], SIGTERM);
    kill(pid[2], SIGTERM);
    kill(mainPid, SIGTERM);



    int usr1GenCounter = 0;
    int usr2GenCounter = 0;

    ifstream iFile("sigGenLog.txt");
    string line;
    while (getline(iFile, line)){
        strstr(line.c_str(), "SIGUSR1") ? usr1GenCounter++ : usr2GenCounter++;
    }

    printf("\nSIGUSR1 signals generated: %i\nSIGUSR2 signals generated: %i\n", usr1GenCounter, usr2GenCounter);
    printf("SIGUSR1 signals received: %i\nSIGUSR2 signals received: %i\n", usr1RecvCounter, usr2RecvCounter);
    printf("SIGUSR1 signal loss: %i/%i = %f%%\n", usr1RecvCounter, usr1GenCounter, (100 * (1 - (double) usr1RecvCounter/usr1GenCounter)));
    printf("SIGUSR2 signal loss: %i/%i = %f%%\n", usr2RecvCounter, usr2GenCounter, (100 * (1 - (double) usr2RecvCounter/usr2GenCounter)));


    ofstream statsFile("sigRecLog.txt", ios_base::app);
    statsFile << "SIGUSR1 signals generated: " << usr1GenCounter << endl << "SIGUSR1 signals received: " << usr1RecvCounter << endl;
    statsFile << "SIGUSR1 signal loss: " << usr1RecvCounter << "/" << usr1GenCounter << " = " << (100 * (1 - (double) usr1RecvCounter/usr1GenCounter)) << "%\n";
    statsFile << "SIGUSR2 signals generated: " << usr2GenCounter << endl << "SIGUSR2 signals received: " << usr2RecvCounter << endl;
    statsFile << "SIGUSR2 signal loss: " << usr2RecvCounter << "/" << usr2GenCounter << " = " << (100 * (1 - (double) usr2RecvCounter/usr2GenCounter)) << "%\n";


    return 0;
}