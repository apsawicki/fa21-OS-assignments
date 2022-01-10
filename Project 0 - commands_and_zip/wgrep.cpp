#include <string.h> // for strstr()
#include <iostream> // cin
#include <fstream>  // file handling
#include <string>   // need string for getline()

// https://www.delftstack.com/howto/cpp/how-to-read-a-file-line-by-line-cpp/
// ^I couldnt use the c library getline for some reason so I used this tutorial to use the iostream one

int main(int argc, char **argv){

    if (argc == 1){ // no keyword to search
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }
    else if (argc == 2){ // no file, input from user
        std:: string line;


        while (getline(std::cin, line)){

            if (strstr(line.c_str(), argv[1])){ // if the line has the keyword then push it into the vector
                printf("%s\n", line.c_str());
            }
        }

    }
    else{ // one or more files given
        for (int i = 2; i < argc; i++){ // from file
            std::string line;

            std::ifstream inFile(argv[i]); // opening file and checking to see if it opened
            if (!inFile.is_open()){
                printf("wgrep: cannot open file\n");
                return 1;
            }

            while (getline(inFile, line)){ // getting the line then seeing if it includes the substring
                if (strstr(line.c_str(), argv[1])){
                    printf("%s\n", line.c_str());
                }
            }

            inFile.close();
        }
    }

    return 0;
}