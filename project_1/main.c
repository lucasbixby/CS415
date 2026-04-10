#include <stdio.h>
#include <string.h>
#include "string_parser.h"
#include "command.h"

// test functions 
int stringParseTest(){

    char* input_string = "pwd ; mkdir test ; cd test sfjlksagjlks;ls; pwd; cd .. ; cd test ; pwd cp ../input.txt . ; ls ; cat input.txt mv input.txt del.txt ; pwd ; ls rm del.txt ; ls cd .. ; pwd ls";

    command_line commands = str_tokenize(input_string);

    for( unsigned int i=0; i < commands.num_token; i++) {
        printf("%s\t", commands.command_list[i]);
    }

    return 0;
}

int main(int argc, char *argv[]) 
{
    // fisrt we must check what mode our program is started in 

    if (stringParseTest() == 0) {
        printf("\nstring parse success\n");
    } else {
        printf("\nstring parse failure\n");
    }

    /* 
    if(argc > 1)
    {
        if(argc == 3)
        {
           // in file mode -> ./psudo-shell -f filename.txt 

        } else {
            fprintf(stderr, "Error! incorrect number of file mode arguments");
            return 1;
        }
    }
    */

    return 0;
}