#include"main.h" // header file
#include<unistd.h> // read and write to fd
#include<string> // strings
#include<pthread.h> // threads
#include<string.h> // for bzero() to clean strings
#include<arpa/inet.h> // for definitions like AF_INET, SOCK_STREAM etc...
#include<queue> // for connections buffer priority queue
#include<algorithm> // to help with removing whitespaces from a string
#include<vector> // need vector for the connections buffer priority queue
#include<chrono> // implementing time
#include<unordered_set> // hashset for the dictionary words
#include<fstream> // reading dict file + writing to dict file
#include<ctime> // seeding srand() random values
using namespace std;
using namespace std::chrono;


priority_queue<CONNECTION, vector<CONNECTION>, my_compare> connectionsBuffer; // connection buffer

queue<string> loggerQueue;

unordered_set<string> dictionary;

bool sequentialBuffer; // lets the threads know whether the user chose sequential or priority buffer
int connectionSize;
int PORT = 2348;



[[noreturn]] void serverThreadFunc(){

    struct sockaddr_in serverAdd, clientAdd;
    int listen_socket, accept_socket;


    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0){ // create the socket file descriptor
         printf("server socket created\n");
    }
    else{
        perror("SOCKET CREATION FAILURE\n");
        exit(EXIT_FAILURE);
    }

    int opt = 1; // I saw online that this was supposed to reduce/prevent binding errors, however I need to test it to make sure it is something useful/doesn't break my program
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    else{
         printf("socket options set\n");
    }

    bzero(&serverAdd, sizeof(serverAdd));
    serverAdd.sin_family = AF_INET; // Internet Protocol
    serverAdd.sin_addr.s_addr = htonl(INADDR_ANY); // I think this sets the address to localhost
    serverAdd.sin_port = htons(PORT); // PORT 8080


    if (bind(listen_socket, (struct sockaddr *)&serverAdd, sizeof(serverAdd)) == 0){ // Attaches socket to the port and address in sockaddr_in struct
         printf("bind success\nPORT: %i\nIP: %s\n", PORT, "localhost");
    }
    else{
        perror("BIND FAILURE\n");
        exit(EXIT_FAILURE);
    }


    if (listen(listen_socket, 5) == 0){ // puts the server socket in listen mode, waits for client to approach the server for a connection
         printf("server listening\n\n");
    }
    else{
        perror("SERVER LISTEN FAILURE\n");
        exit(EXIT_FAILURE);
    }


    socklen_t len = sizeof(clientAdd);
    while(true){ // the server will continuously wait for clients to connect and then add them to the queue
        accept_socket = accept(listen_socket, (struct sockaddr*)&clientAdd, &len);
        if (accept_socket >= 0){
            // printf("client detected\n");
            auto startTime = high_resolution_clock::now();

            pthread_mutex_lock(&connectionMTX);
            CONNECTION con{accept_socket, startTime, 0};

            while(connectionsBuffer.size() == (unsigned int) connectionSize){
                pthread_cond_wait(&emptySpace, &connectionMTX);
            }

            if (sequentialBuffer){
                connectionsBuffer.push(con);
            }
            else{
                con.priority = (rand() % 10) + 1;
                connectionsBuffer.push(con);
            }

            printf("client accepted - fd: %i\n", accept_socket);

            pthread_cond_signal(&nonEmptyQueue);
            pthread_mutex_unlock(&connectionMTX);
        }
    }

} // Main Server Thread (Producer) - doesn't need locking mechanisms since it is the only thread that will produce for it's consumers

[[noreturn]] void *loggerThreadFunc(void *arg){

    // The log buffer entry is to contain the arrival time of the request, the time the spell check was completed,
    // the word checked, the result of the spell check and the priority of the request

    while (true){

        string log;

        pthread_mutex_lock(&loggerMTX);
        while(loggerQueue.empty()){
            pthread_cond_wait(&nonEmptyLogQueue, &loggerMTX);
        }

        log = loggerQueue.front();
        loggerQueue.pop();

        pthread_mutex_unlock(&loggerMTX);

        printf("log: %s\n", log.c_str());
        ofstream outFile("logfile.txt", ios_base::app);
        outFile << log;
        outFile.close();


        // we have the struct with log information, we need to extract the info and write to a file
    }
}

[[noreturn]] void *workerThreadFunc(void *arg){
    while (true){

        CONNECTION client;
        pthread_mutex_lock(&connectionMTX);

        while (connectionsBuffer.empty()){
            pthread_cond_wait(&nonEmptyQueue, &connectionMTX); // wait until the connection queue has something in it and then we will start working
        }

        client = connectionsBuffer.top();
        connectionsBuffer.pop();
        int client_fd = client.clientFD;

        // the client fd is now being serviced so i will stop the timer
        auto servicedTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(servicedTime - client.requestTime);
        string requestServiced = to_string(duration.count());


        pthread_cond_signal(&emptySpace);
        pthread_mutex_unlock(&connectionMTX);

        while(true){
            char buff[80]; // message maximum length is 80
            string serverMessage;
            string log;
            // read client message
            read(client_fd, buff, sizeof(buff));
            serverMessage = buff;
            serverMessage.erase(remove(serverMessage.begin(), serverMessage.end(), ' '), serverMessage.end()); // should remove all whitespace from the string
            serverMessage.erase(remove(serverMessage.begin(), serverMessage.end(), '\n'), serverMessage.end()); // should remove newline character at end of string
            bzero(buff, 80);

            if (strlen(serverMessage.c_str()) != 0){
                printf("Client: %i : (%s)\n", client_fd, serverMessage.c_str());

                if (serverMessage == "exit"){ // lets the client leave without breaking the pipe with the server(which would cause the server thread to stop)
                    close(client_fd);
                    break;
                }

                // dictionary checking
                if (dictionary.find(serverMessage) == dictionary.end()){
                    serverMessage += " MISSPELLED";
                }
                else{
                    serverMessage += " OK";
                }
                // the spell check was just finished so i will record the time in regards to when the client was first serviced by the worker thread
                auto spellTime = high_resolution_clock::now();
                duration = duration_cast<microseconds>(spellTime - servicedTime);
                string spellCheckTime = to_string(duration.count());


                pthread_mutex_lock(&loggerMTX);
                // adding to logger queue
                log.append(serverMessage + " " + to_string(client.priority) + " : " + requestServiced + " " + spellCheckTime + "\n");
                loggerQueue.push(log);

                pthread_cond_signal(&nonEmptyLogQueue);
                pthread_mutex_unlock(&loggerMTX);

                // write spell checked word to client
                serverMessage += "\n";
                write(client_fd, serverMessage.c_str(), serverMessage.size());


            }
        }
    }

} // Worker Threads (Consumer)


void spawnThreads(int workerThreadCount){

    // create logger thread
    pthread_t loggerThread;
    if(pthread_create(&loggerThread, nullptr, loggerThreadFunc, nullptr) != 0){
        printf("Error: failed to create logger thread\n");
        exit(EXIT_FAILURE);
    }


    // create worker threads
    pthread_t workerThreads[workerThreadCount];
    for (int i = 0; i < workerThreadCount; i++){
        if (pthread_create(&workerThreads[i], nullptr, workerThreadFunc, nullptr) != 0){
            printf("Error: failed to create worker thread\n");
            exit(EXIT_FAILURE);
        }
    }
} // Creates the worker and logger threads

void getDict(string dict){

    string line;
    ifstream inFile;

    dict.empty() ? inFile.open("dict") : inFile.open(dict);

// im starting to try and use ternary operators - the above line should be equal to what is commented below
//    if (dict.empty()){
//        inFile.open("dict");
//    }
//    else{
//        inFile.open(dict);
//    }

    if (!inFile.is_open()){
        perror("dict");
    }

    while (getline(inFile, line)){
        dictionary.insert(line);
    }
    inFile.close();
} // opens the dictionary file provided or a default dictionary if no dict provided

bool isNumber(string str){
    for (char c : str){
        if (!isdigit(c)){
            return false;
        }
    }
    return true;
} // checks if my arguments are numbers or not

int checkArgs(int argc, char** argv, string& dict, int& workerThreadCount){
    int counter = 1;
    if (argc == 5){
        if (isNumber(string(argv[1]))){
            PORT = atoi(argv[1]);
        }
        else{
            dict = string(argv[1]);
        }

        counter++;
    }
    else if (argc == 6){
        if (!isNumber(string(argv[1])) || isNumber(string(argv[2]))){
            printf("port/dict value specified not valid\n");
            return 1;
        }
        PORT = atoi(argv[1]);
        dict = string(argv[2]);
        counter += 2;
    }

    if (argc > 3){
        connectionSize = atoi(argv[counter++]);
        workerThreadCount = atoi(argv[counter++]);
        sequentialBuffer = atoi(argv[counter]);
    }
    else{
        printf("Must provide at least three command line arguments: ./main [optional port number] [optional dictionary file] [buffer cells number] [worker thread number] [scheduling type]\n");
        return 1;
    }


    return 0;
} // checks the program args to see if they are valid


int main(int argc, char** argv){

    srand(time(NULL));


    string dict = "";
    int workerThreadCount;


    if (checkArgs(argc, argv, dict, workerThreadCount)){ // will check if the arguments given are valid
        return 1;
    }

    getDict(dict); // gets all of the words in the dictionary into a global unordered_set

    printf("worker thread count: %i\n", workerThreadCount);
    printf("connection buffer size: %i\n", connectionSize);
    printf("sequential queue: %d\n", sequentialBuffer);
    printf("port number: %i\n", PORT);
    dict.empty() ? printf("dict name: dict\n\n") : printf("dict name: %s\n\n", dict.c_str());


    spawnThreads(workerThreadCount); // spawns worker + log threads
    serverThreadFunc(); // server thread starts



    return 0;
}
