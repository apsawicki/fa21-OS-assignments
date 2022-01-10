#include <stdio.h>
#define LINE_MAX 1024 // max character limit for notepad in windows


int main(int argc, char **argv){

    for (int i = 1; i < argc; i++){

        FILE *inFile = fopen(argv[i], "r"); // opening file
        if (inFile == NULL){
            printf("wcat: cannot open file\n");
            return 1;
        }
        char line[LINE_MAX];

        while (fgets(line, LINE_MAX, inFile) != NULL){
            printf("%s", line);
        }

        fclose(inFile);
    }

    return 0;
}