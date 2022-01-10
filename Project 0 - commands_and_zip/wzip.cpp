#include <stdio.h>

int main(int argc, char **argv){

    if (argc == 1){
        printf("wzip: file1 [file2 ...]\n");
    }

    for (int i = 1; i < argc; i++){

        char letter[1];
        // "/mnt/c/Users/apsaw/CLionProjects/project-0-fall21-apsawicki/testfile.txt"
        FILE *inFile = fopen(argv[i], "r"); // read file
        if (inFile == NULL){
            printf("wzip: cannot OPEN file\n");
            return 1;
        }

        int count = 0;
        char pastLetter = fread(letter, sizeof(letter), 1, inFile); // gets the first char and rewinds the ptr afterwards
        rewind(inFile);


        while ((fread(letter, sizeof(letter), 1, inFile)) == 1){ //read the contents of the file, one char at a time
            //printf(">%c",letter[0]);

            count++;

            if (pastLetter != letter[0]){

                fwrite(&count, 4, 1, stdout); // writing int
                fwrite(&pastLetter, 1, 1, stdout); // writing the char

                count = 0;

                //            printf("%i", count);
//            if (pastLetter == '\r'){
//                printf("'\\r' \n");
//            }
//            else if (pastLetter =='\n'){
//                printf("'\\n' \n");
//            }
//            else{
//                printf("'%c' \n", pastLetter);
//            }
            }

            pastLetter = letter[0];
        }

        count++;

        fwrite(&count, 4, 1, stdout);
        fwrite(&pastLetter, 1, 1, stdout); // writing the char

        //    printf("%i", count);
        //    printf("'%c' \n", pastLetter);

    }

    return 0;
}
