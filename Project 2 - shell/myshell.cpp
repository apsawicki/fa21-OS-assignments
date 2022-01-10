#include<cstdio> // standard c lib
#include<iostream> // for getline and i/o
#include<string> // use strings
#include<bits/stdc++.h> // for strtok()
#include<dirent.h> // for dirent/directory
#include<unistd.h> // dup2() and piping/fd functions
#include<fcntl.h> // i/o redirection
#include<wait.h> // waitpid
#include<vector> // for vectors
#include<algorithm> // for copy() with arrays

using namespace std;


bool arrContains(string* arr, int arrSize, string val){ // function to check if a string arr contains a string and returns t/f

    for (int i = 0; i < arrSize; i++){
        if (arr[i] == val){
            return i + 1; // will return > 0 if it finds the val in the array (which means it will return true)
        }
    }

    return 0; // return 0 (means false)
}

int findWordCount(string line){ // gets the wordCount (arraySize) of the string

    int wordCount = 0;

    char *token = strtok(&line[0], " ");
    while (token != NULL) { // while there are more words, counter++
        wordCount++;
        token = strtok(NULL, " ");
    }

    // printf("wordcount: %i\n", wordCount);
    return wordCount;
}

string* parseTokens(string line, int wordCount){ // turns given string into an array delimited by spaces

    string *tokens = new string[wordCount]{};
    int i = 0;

    char* token = strtok(&line[0], " "); // adds the strings delimited by a space to the array
    while (token != NULL) {
        // printf("(%s)\n", token);
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    return tokens;
}

int* parseCommands(int wordCount, string* tokens, string* args){ // parses the word array for commands, then saves the index of the commands in commands[]

    int *commands = new int[wordCount]; // will store the index of each command (meaning no arguments)
    commands[0] = 0; // accounting for the first command
    int commandsIndex = 1;

    for (int j = 0; j < wordCount; j++){ // storing all the commands

        // making sure that there is correct usage for the io redirection, background execution, and piping arguments
        if (j != (wordCount - 1)){
            if (tokens[j] == ">" && arrContains(args, 5, tokens[j + 1])){
                perror("incorrect usage of \">\", use command \"help\" for correct usage");
            }

            if (tokens[j] == ">>" && arrContains(args, 5, tokens[j + 1])){
                perror("incorrect usage of \">>\", use command \"help\" for correct usage");
            }

            if (tokens[j] == "<" && arrContains(args, 5, tokens[j + 1])){
                perror("incorrect usage of \"<\", use command \"help\" for correct usage");
            }

            if (tokens[j] == "&" && arrContains(args, 5, tokens[j + 1])){
                perror("incorrect usage of \"&\", use command \"help\" for correct usage");
            }

            if (tokens[j] == "|" && arrContains(args, 5, tokens[j + 1])){
                perror("incorrect usage of \"|\", use command \"help\" for correct usage");
            }
        }
        else{
            // printf("end\n");
            if (tokens[j] == "<"){
                perror("incorrect usage of \"<\", use command \"help\" for correct usage");
            }

            if (tokens[j] == ">"){
                perror("incorrect usage of \">\", use command \"help\" for correct usage");
            }

            if (tokens[j] == ">>"){
                perror("incorrect usage of \">>\", use command \"help\" for correct usage");
            }

            if (tokens[j] == "|"){
                perror("incorrect usage of \"|\", use command \"help\" for correct usage");
            }
        }

        // recognizing commands
        if ((tokens[j] == "&" && j != (wordCount - 1)) || (tokens[j] == "|" && j != (wordCount - 1))){ // any char that will be put before a command is run should be compared here to find all commands

            commands[commandsIndex] = j + 1; // since a second command will only be after one of those flags
            // printf("commandIndex: %i", (j + 1));
            commandsIndex++;
            // printf("command: %s\n", tokens[j + 1].c_str());
        }

    }
    commands[commandsIndex] = -1; // signal the end of the commands by setting the last one to -1

    return commands;
}

string* fileRedirection(string* commandArr, int& arrSize, string* args, bool &background, int* fdTable, bool& pipeStatus){ // if there is input, will return an array of the input concatenated right after the command as arguments, if output, will mess with the fd table

    if(pipeStatus){
        dup2(fdTable[0], STDIN_FILENO);
        pipeStatus = false;
    }

    if (arrSize > 1){
        for (int i = 1; i < arrSize; i++){
            if (commandArr[i] == "<"){ // input

                int new_fd = open(commandArr[i + 1].c_str(), O_RDONLY);
                dup2(new_fd, STDIN_FILENO);
                close(new_fd);

            }
            else if (commandArr[i] == ">"){ // write output

                int new_fd = open(commandArr[i + 1].c_str(), O_WRONLY | O_CREAT);
                dup2(new_fd, 1);
                close(new_fd);

            }
            else if (commandArr[i] == ">>"){ // append output

                int new_fd = open(commandArr[i + 1].c_str(), O_WRONLY | O_APPEND | O_CREAT);
                dup2(new_fd, 1);
                close(new_fd);

            }
            else if (commandArr[i] == "&"){ // background execution

                background = true;

            }
            else if (commandArr[i] == "|" ){ // pipe

                if((pipe(fdTable)) == -1){
                    perror("Pipe failed");
                    return commandArr;
                }
                else{
                    dup2(fdTable[1], STDOUT_FILENO);
                    pipeStatus = true;
                }
            }
        }
    }


    // im only going to return the arguments without the file redirection, piping, and background execution args as i will handle those here
    int newCommandArrSize = arrSize;
    for (int i = 0; i < arrSize; i++){
        if (arrContains(args, 5, commandArr[i])){
            newCommandArrSize = i;
            break;
        }
    }
    arrSize = newCommandArrSize;
    string* newCommandArr = new string[newCommandArrSize];
    copy(commandArr, commandArr + newCommandArrSize, newCommandArr);

    // newCommandArr will have the command + arguments without file redirection strings
    return newCommandArr;
}

string* commandArrInit(int& commandArrSize, int* commands, int i, int wordCount, string* tokens){ // returning an array with only one command and it's arguments

    string* commandArr;

    // getting the size for the commandArr
    if (commands[i + 1] == -1){ // if it is the last command in the tokens array then i have to get the size differently
        commandArrSize = wordCount - commands[i];
    }
    else{
        commandArrSize = commands[i + 1] - commands[i];
    }


    commandArr = new string[commandArrSize];
    copy(tokens + commands[i], tokens + commands[i] + commandArrSize, commandArr);

    return commandArr;
}


int cdFunc(string* commandArr, int arrSize, string* args){

    // getting the current directory for dir so we can print it if there are no arguments
    printf("\n");
    char path[81]; // 81 is command line char max length
    DIR *dir = opendir(getcwd(path, 80));
    struct dirent *dirInfo;

    // printf("current dir: %s", getcwd(path, 80));

    if (arrSize == 1 || arrContains(args, 5, commandArr[1])){ // 0 arguments - if its just cd, then print from cwd
        if (dir != NULL){
            while ((dirInfo = readdir(dir)) != NULL){
                printf("%s\n", dirInfo->d_name);
            }
        }
        else{
            perror("error opening cur dir: ");
            return 1;
        }
    }
    else{ // 1 argument - change directory to the first arg and change PWD value to current path
        char newdir[1000];
        if (chdir(commandArr[1].c_str()) == -1){
            perror("not a correct directory path");
        }
        setenv("PWD", getcwd(newdir, 1000), 1);
        // printf("%s\n", getcwd(path, 80));
    }

    closedir(dir);
    return 0;
} // fully baked

int dirFunc(string* commandArr, int arrSize, string* args){

    // getting current dir
    char path[81]; // 81 is command line char max length
    DIR *dir = opendir(getcwd(path, 80));
    struct dirent *dirInfo;

    if (arrSize == 1 || arrContains(args, 5, commandArr[1])){ // 0 arguments
        if (dir != NULL){ // printing out the cwd dir if there are no args
            while ((dirInfo = readdir(dir)) != NULL){
                printf("%s\n", dirInfo->d_name);
            }
        }
        else{
            perror(commandArr[1].c_str());
            return 1;
        }

    }
    else{ // > 1 argument

        if (commandArr[1].c_str()[0] == '/'){ // absolute path
            if ((dir = opendir(commandArr[1].c_str())) != NULL){
                while ((dirInfo = readdir(dir)) != NULL){
                    printf("%s\n", dirInfo->d_name);
                }
            }
            else{
                perror(commandArr[1].c_str());
                return 1;
            }
        }
        else{ // relative path
            string output = string(path) + "/" + commandArr[1];
            if ((dir = opendir(output.c_str())) != NULL){
                while ((dirInfo = readdir(dir)) != NULL){
                    printf("%s\n", dirInfo->d_name);
                }
            }
            else{
                perror(commandArr[1].c_str());
                return 1;
            }
        }
    }


    closedir(dir);
    return 0;
} // fully baked

int environFunc(char** envp){

    // prints out all the environment vars by getting the envp pointer and getting the values from it while it isn't NULL
    char ** env = envp;
    while (*env != NULL){
        printf("%s\n", *env);
        env++;
    }

    return 0;
} // fully baked

int echoFunc(string* commandArr, int arrSize, string* args){

    // if there are no args then we can't use echo to print anything
    if (arrSize == 1 || arrContains(args, 5, commandArr[1])){
        perror("incorrect usage: echo [string]");
        return 1;
    }

    // for each arg we print it and separate them by a space
    for (int i = 1; i < arrSize; i++){
        if (arrContains(args, 5, commandArr[i])){
            break;
        }
        printf("%s ", commandArr[i].c_str());
    }
    printf("\n");

    return 0;
} // need to check if "" are used, then will output only whats in the ""

int helpFunc(){

    // read the readme_doc and then print it out line by line
    string line;
    ifstream inFile("readme_doc");
    if (!inFile.is_open()){
        perror("readme_doc");
        return 1; // error
    }

    while (getline(inFile, line)){
        printf("%s\n", line.c_str());
    }
    printf("\n");


    return 0;
} // fully baked

int pauseFunc(){

    // busy-wait in a while loop waiting for enter to be pressed
    char c;
    printf("press enter to resume processes");

    while ((c = getchar())){
        if (c == '\n'){
            break;
        }
    }

    return 0;
} //fully baked

int pathFunc(string* commandArr, int arrSize){

    string pathStr = "";

    //if there are no args then the PATH will be set to ""
    if (arrSize > 1){
        pathStr = commandArr[1];
    }

    // for each arg we will put a colon between them and then add it to the string
    for (int i = 2; i < arrSize; i++) {
        pathStr += ":" + commandArr[i];
    }

    // set PATH to the string we made
    setenv("PATH", pathStr.c_str(), 1);

    return 0;
}

int builtInCommand(string* commandArr, int arrSize, string* args, char** envp, int* fdTable, bool& pipeStatus){ // will run the built-in commands

    bool background = false;

    // saving fd so we can reset them later
    int stdOutSave = dup(STDOUT_FILENO);
    int stdInSave = dup(STDIN_FILENO);

    commandArr = fileRedirection(commandArr, arrSize, args, background, fdTable, pipeStatus);
    // printf("commandArr 0 : %s", commandArr[0].c_str());

    // if we need background execution, then i need to fork() to make a background process
    pid_t pid;
    if (background){
        pid = fork();

        string error = "Error occurred while forking command (" + commandArr[0] + ") : ";
        if (pid < 0){
            printf("error forking");
            perror(error.c_str());
            return 1;
        }
        if (pid > 0){ // i want the parent to continue doing shell commands so it will return
            // printf("\n\nbuilt in parent\n\n");
            fflush(stdout);
            fflush(stdin);
            dup2(stdOutSave, STDOUT_FILENO);
            dup2(stdInSave, STDIN_FILENO);
            close(stdOutSave);
            close(stdInSave);
            return 0;
        }
    }

    // if statements to chrck which built-in command it is
    if (commandArr[0] == "cd"){
        cdFunc(commandArr, arrSize, args);
    }
    else if (commandArr[0] == "clr"){
        printf("\033c");
    }
    else if (commandArr[0] == "dir"){
        dirFunc(commandArr, arrSize, args);
    }
    else if (commandArr[0] == "environ"){
        environFunc(envp);
    }
    else if (commandArr[0] == "echo"){
        echoFunc(commandArr, arrSize, args);
    }
    else if (commandArr[0] == "help"){
        helpFunc();
    }
    else if (commandArr[0] == "pause"){
        pauseFunc();
    }
    else if (commandArr[0] == "quit"){
        exit(EXIT_SUCCESS);
    }
    else if (commandArr[0] == "path"){
        pathFunc(commandArr, arrSize);
    }
    else{
        printf("ERROR");
        exit(EXIT_FAILURE);
    }

    // make sure to reset all the fd values
    fflush(stdout);
    fflush(stdin);
    dup2(stdOutSave, STDOUT_FILENO);
    dup2(stdInSave, STDIN_FILENO);
    close(stdOutSave);
    close(stdInSave);

    // close pipe so that any waitpid() can return the child process since it wont be tied up with the pipe
    if (pipeStatus){
        close(fdTable[1]);
    }

    if (pid == 0){ // if i did background execution, i dont want the child to continue back to the shell and make another copy of the shell running, so i will exit the child before it gets back to the shell
        // printf("child\n");
        exit(0);
    }

    return 0;
}

int extCommand(string* commandArr, int arrSize, string* args, int* fdTable, bool& pipeStatus){ // will run external commands (non built-in commands)

    bool background = false;

    // saving fd so i can reset them later
    int stdOutSave = dup(STDOUT_FILENO);
    int stdInSave = dup(STDIN_FILENO);

    commandArr = fileRedirection(commandArr, arrSize, args, background, fdTable, pipeStatus);

    int pid = fork();

    string error = "Error occurred while forking command (" + commandArr[0] + ") : ";
    if (pid < 0){
        // printf("error forking");
        perror(error.c_str());
    }
    else if (pid == 0){ // child

        // I wasn't able to find a way to convert a string array to a c_str array without using a vector like this
        vector<char *> vec;
        for (int i = 0; i < arrSize; i++) {
            vec.push_back((char *) commandArr[i].c_str()); // i need to cast it to char * because push_back() wants a constant
        }
        vec.push_back(NULL); // need to push NULL so the c array is null terminated

        if(execvp(commandArr[0].c_str(), &vec[0]) == -1){
            string errorMsg = "Error while exevp() on command " + commandArr[0];
            perror(errorMsg.c_str());
        }

    }
    else if (pid > 0){ // parent

        if (!background){ // background execution for ext commands, basically if we just dont wait for the child then background execution is being used


            if (pipeStatus){ // i have to close the pipe, otherwise, waitpid() wont return because the child is being tied up with the pipe
                close(fdTable[1]);
            }
            waitpid(pid, NULL, P_ALL);// non blocked

        }
    }

    // resetting the fd
    fflush(stdout);
    fflush(stdin);
    dup2(stdOutSave, STDOUT_FILENO);
    dup2(stdInSave, STDIN_FILENO);
    close(stdOutSave);
    close(stdInSave);

    return 0;
}


int main(int argc, char** argv, char** envp){


    // setting up the SHELL path to accommodate my shell
    char shellPath[1000];
    getcwd(shellPath, 1000);
    setenv("SHELL", strcat(shellPath, "/myshell"), 1);

    // arrays that will be used with the arrContains() function
    string builtIn[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause", "quit", "path"}; // size 9
    string args[] = {"<", ">", ">>", "&", "|"}; // size 5

    while (true) {

        // setting up vars for tokenizing user input, and for piping
        char curDir[1000];
        int wordCount;
        string *tokens;
        int *commands;
        string line;
        int fdTable[2]; // for pipe
        bool pipeStatus = false;

        if (argc == 1) { // parsing regular command line

            // getting user input, we dont want more than 80 chars
            printf("%s>", strcat(getcwd(curDir, 1000), "/myshell"));
            getline(cin, line);

            while (line.size() > 80) {
                printf("commands with more than 80 chars not allowed\n>");
                printf("%s>", strcat(getcwd(curDir, 1000), "/myshell"));
                getline(cin, line);
            }

            while (line.size() == 0){
                printf("%s>", strcat(getcwd(curDir, 1000), "/myshell"));
                getline(cin, line);
            }

            // tokenizing input
            wordCount = findWordCount(line);
            tokens = parseTokens(line, wordCount);
            commands = parseCommands(wordCount, tokens, args);

            int i = 0;
            while (commands[i] != -1){

                // getting the singular command and it's arguments in one array
                int commandArrSize;
                string* commandArr = commandArrInit(commandArrSize, commands, i, wordCount, tokens);

                // going to built-in commands and external commands
                if (arrContains(builtIn, 9, commandArr[0])){

                    builtInCommand(commandArr, commandArrSize, args, envp, fdTable, pipeStatus);
                }
                else{
                    extCommand(commandArr, commandArrSize, args, fdTable, pipeStatus);
                }
                i++;
            }

        } else if (argc == 2) { // parsing batch file

            // reading the batch file
            ifstream iFile (argv[1]);
            if (!iFile.is_open()){
                perror("could not open batch file");
                return 1;
            }

            while (getline(iFile, line)) {
                if (line.size() > 80){
                    printf("commands with more than 80 chars not allowed\n");
                    return 1;
                }
                if (line.size() == 0){
                    return 0;
                }

                // I tokenize and run one line at a time from the batch file, so once it is done getting lines, it will return 0
                wordCount = findWordCount(line);
                tokens = parseTokens(line, wordCount);
                commands = parseCommands(wordCount, tokens, args);

                int i = 0;
                while (commands[i] != -1){

                    // getting the singular command and it's arguments
                    int commandArrSize;
                    string* commandArr = commandArrInit(commandArrSize, commands, i, wordCount, tokens);

                    // going to built-in and external commands
                    if (arrContains(builtIn, 9, commandArr[0])){ // if the command is a built-in command
                        builtInCommand(commandArr, commandArrSize, args, envp, fdTable, pipeStatus);
                    }
                    else{
                        extCommand(commandArr, commandArrSize, args, fdTable, pipeStatus);
                    }
                    i++;
                }

            }
            // will return once there are no more lines to read in the batch file
            return 0;
        }
    }
}