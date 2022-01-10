// Shell Pseudo-Code
#include<cstdio> // standard c lib
#include<iostream> // for getline and i/o
#include<string> // use strings
#include<bits/stdc++.h> // for the unordered set
#include<dirent.h> // for dirent/directory
#include<unistd.h> // fork, execvp, pipe, dup2


cdFunc(tokens, dirent struct){

    if argc == 1{ // only cd command, no arguments
        print working directory from dirent struct, maybe with _getcwd() ?
    }
    else if argc == 2{ // cd command with directory we want to move to

        if the path exists{
            use chdir(tokens[1]) to change the working directory
            also setenv() so that SHELL is currently <current path>/myshell/<whatever directory we change to>
        }
        otherwise tell the user the path doesnt exist // not sure if we will actually have a path that doesnt exist or if we are just adding directory to the dirent struct
    }

}

dirFunc(tokens, dirent struct, fd (file descriptor array pipe table)){

    if tokens contains both "|" and ">" and ">>" then print an error

    if pipeStatus == true{

        get the input from the fd[] arrays read section using read(), then turn it into tokens and append the tokens to tokens[] array in args right after our command
        pipeStatus = false;
    }
    else if tokens.size() > 1 && tokens contains a "<"{

        get the input from the input file(which should be right after the < symbol) and turn the input into tokens delimited by a space,
        then append the tokens found to tokens[] array from our arguments right after the command token

    }

    if tokens.size() > 1 && tokens contains ">"or">>"or"|"{

        if >: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token
                then wipe the file



        if >>: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token



        if |: we will write to the fd[] array in the args (using write())and signify that the next command will be taking in input from the fd array by setting pipeStatus = true;
    }

    use opendir(on the file specified)  then compare the dirent structs info with the readdir(on the file specified) while it doesnt equal null
    and printf all of the values that the opendir variable gets, and when it printfs it will go to the 0 slot on the file descriptor table and
    since it is open to the new file it will output to the new file
    however,
    if pipeStatus is currently true, then we will write() to the fd[] table instead of printing for each printf


}

environFunc(tokens, environ, fd){

    if tokens contains both "|" and ">" and ">>" then print an error

    if pipeStatus == true{

        get the input from the fd[] arrays read section using read(), then turn it into tokens and append the tokens to tokens[] array in args right after our command
        pipeStatus = false;
    }
    else if tokens.size() > 1 && tokens contains a "<"{

        get the input from the input file(which should be right after the < symbol) and turn the input into tokens delimited by a space,
        then append the tokens found to tokens[] array from our arguments right after the command token

    }

    if tokens.size() > 1 && tokens contains ">"or">>"or"|"{

        if >: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token
            then wipe the file

        if >>: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token


        if |:  we will write to the fd[] array in the arguments (using write()) and signify that the next command will be taking in input from the fd array by setting pipeStatus = true;

    }


    if tokens.size() == 1{
        for every name in environ array, printf(getenv(name))
    }

    if tokens.size() > 1 && there is an argument that doenst equal ">", ">>" or "|" and there arent the tokens right after > and >>{
        check if the string contains a "=", otherwise it isnt correct syntax for the command

        delimit the string by the = to get it in terms of the env var and the value you want to set it to set it to
        use setenv(env var name, value, 1); // 1 is to overwrite the old value

        check for errors and print if there are any

        otherwise i will print everything using printf so if the fd table had something written to it then it will go to that value

        however,
        if pipeStatus is currently true, then we will write() to the fd[] table instead of printing for each printf

    }

    then reset the fd table to its original value

}

echoFunc(tokens, fd){

    if tokens contains both "|" and ">" and ">>" then print an error

    if pipeStatus == true{

        get the input from the fd[] arrays read section using (read()), then turn it into tokens and append the tokens to tokens[] array in args right after our command token
        pipeStatus = false;
    }
    else if tokens contains a "<"{
        get the input from the input file(which should be right after the < symbol) and turn the input into tokens delimited by a space,
        then append the tokens found to tokens[] array from our arguments right after the command token
    }


    if tokens contains ">"or">>"or"|"{

        if >: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token
            then wipe the file

        if >>: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token


        if |:  we will write to the fd[] array in the arguments (using write()) and signify that the next command will be taking in input from the fd array by setting pipeStatus = true;


    }

    printf all of the arguments (then print a space after each arg) until you hit either ">"or">>"or"|"or"<" or the end of the tokens arr
    // this will print to whatever is in the stdout file descriptor slot at the moment

    then reset the fd table to its original value
    however,
    if pipeStatus is currently true, then we will write() to the fd[] table instead of printing

}

helpFunc(tokens, fd){

    if pipeStatus == true{ // we shouldnt take any input from anywhere else
        pipeStatus = false;
    }

    if tokens contains ">"or">>"or"|"{ // but output should be allowed

        if >: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token
        then wipe the file

        if >>: use dup2() on stdout to save it, then write the 0 slot on the fd table to whatever the token/file is that is after the > token
        then printf all output(make sure to append)

        if |:  we will write to the fd[] array in the arguments (using write()) and signify that the next command will be taking in input from the fd array by setting pipeStatus = true;
    }

    read the manual/help file that we will be creating to explain the shell and how to use it, the printf the entire manual
    however,
    if pipeStatus is currently true, then we will write() to the fd[] table in the args instead of printing


}


int main(int argc, char** argv, char **envp){

    bool status = true;
    make a dirent struct/var that will hold the path type_info
    fd[2], these will hold the file descriptors for
    use pipe() command on fd[] array as to setup the piping before the commands start

    bool pipeStatus = false; // will be used to tell a command whether to read from the pipe or not

    while (status){

        if argc == 1{
            // get the line
            string line;


            getline(cin, line);

            while (line.size() > 80){
                printf("too many chars, use 80 or less\n%s>", environment.c_str());
                getline(cin, line);
            }

            // delimit the line into tokens and add those tokens to a string array
            string temp = line;
            char sep[] = " ";
            char *token;
            int wordCount = 0;

            token = strtok(&temp[0], " ");
            while (token != NULL){
                wordCount++;
                token = strtok(NULL, " ");
            }

            string* tokens = new string[wordCount] {}; // will hold all the strings(one word per index) delimited by spaces
            int i = 0

            token = strtok(&line[0], " ");
            while (token != NULL){
                tokens[i] = token;
                i++;
                token = strtok(NULL, " ");
            }


            int* commands = new int[wordCount]; // will store the index of each command (meaning no arguments)
            commands[0] = 0; // accounting for the first command
            commandsIndex = 1;

            for (int i = 0; i < tokens.size(); i++){ // storing all the commands

                if ((tokens[i] == "&" || word == "|")){ // any char that will be put before a command is run should be compared here to find all commands

                    commands[commandsIndex] = i + 1; // since a second command will only be after one of those flags
                    commandsIndex++;

                }

            }
        }
        else if argc == 2{

            read from the file given and make the tokens and commands arrays as shown when argc == 1
            but each newline will also mark a new command in the commands array

        }


        // at this point, commands[] should hold the index of each command in words[]



        // now i will run each command through this for loop
        for (int i = 0; i < commands.size(); i++){

            make "commandArr" a new array that will have all of the arguments for its command so i can feed it to functions
            using some sort of array copy that will only copy the values i tell it

            string command = tokens[commands[i]]; // will be the command we are currently working on



            //built-in commands
            switch (command){
                case "cd" :{

                    cdFunc(commandArr, dirent struct);
                    break;
                }
                case "clr" :{

                    will ignore all arguments
                    clear the screen
                    break;
                }
                case "dir" :{ // can use > >>

                    dirFunc(commandArr, dirent struct, fd);
                    break;
                }
                case "environ" :{

                    environFunc(commandArr, environ, fd);
                    break;
                }
                case "echo" :{

                    echoFunc(commandArr, fd);
                    break;
                }
                case "help" :{

                    helpFunc(commandArr, fd);
                    break;
                }
                case "pause" :{ // DONE

                    will pause stay paused until enter is pressed, something like the following lines:

                    string ENTER;
                    printf("To continue press enter...");

                    while (true){
                        if (getline(cin, string) == "\n"){
                            break;
                        }
                    }

                    break;
                }
                case "quit" :{ // DONE
                    return 0;
                    break;
                }
                default: { // non built-in commands

                    if we are taking input from another file{ // <

                        use dup2() to save the stdin fd and then overwrite the stdin slot with the fd of the input file using write()
                        then we will add all the input right after our command in the commandsArr

                    }
                    else if we are taking input from a commands pipe{ // | if pipeStatus == true

                        read from fd[] array using read() to get the input from the last command
                        then we will add all the input right after our command in the commandsArr

                        pipeStatus = false;
                    }

                    if we are outputting to somewhere other than stdout{

                        if output == ">"{ // meaning we overwrite all data in the output file

                            clear the output file
                            use dup2() to save the stdout fd and then overwrite the stdout slot with the fd of the output file using write()

                        }
                        else if output == ">>"{ // meaning we just append

                            use dup2() to save the stdout fd and then overwrite the stdout slot with the fd of the output file using write()

                        }
                        else if output == "|"{ // meaning we will be using pipe to move our output

                            write to the fd[] array using write() to pipe the output of the current command and i think we change save/change stdout as the fd[] write portion
                            pipeStatus = true;

                        }
                    }

                    pid = fork();

                    if (id < 0){ // error
                        printf an error
                    }

                    if (the very last argument in commandArr != "&"){

                        if child then{
                            execpv() with the command and then an array of all the arguments as function args

                            then output all the output
                            // no wait()
                        }

                    }
                    else{ // normal fork() exec() flow

                        use execpv() to run the non built-in command for the child process
                        // since we already messed with the i/o redirection and piping, the fd table for the child will hold where it needs to output depending on what happens
                        // in the above if statements and the child will ouput to where we set the fd stdout descriptor

                        then have the parent process wait for the child to be done
                        // since it isn't a background command we will wait for the child and then restart the loop depending on how many commands are left
                    break;
                }
        }
    }

    return 0;
}