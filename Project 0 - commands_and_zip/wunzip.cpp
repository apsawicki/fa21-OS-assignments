#include <stdio.h>

int main(int argc, char **argv){

    if (argc == 1){
        printf("wunzip: file1 [file2 ...]\n");
    }

    for (int i = 1; i < argc; i++){

     FILE *inFile = fopen(argv[i], "r");
        if (inFile == NULL){
            printf("wunzip: cannot OPEN file\n");
            return 1;
        }


        while(true){

            int num;
            char letter;

            if ((fread(&num, 4, 1, inFile) != 1) || (fread(&letter, 1, 1, inFile) != 1)){
                break;
            }

            for (int j = 0; j < num; j++){
                printf("%c", letter);
            }
        }
    }

    return 0;
}