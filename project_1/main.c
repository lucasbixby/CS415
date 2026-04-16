#include <stdio.h>
#include <string.h>
#include "string_parser.h"
#include "command.h"

// test functions 
int stringParseTest(){

    char* input_string = "pwd ; mkdir test ; cd test sfjlksagjlks;ls; pwd; cd .. ; cd test ; pwd cp ../input.txt . ; ls ; cat input.txt mv input.txt del.txt ; pwd ; ls rm del.txt ; ls cd .. ; pwd ls";

    command_line commands = str_tokenize(input_string);

    for( unsigned int i=0; i < commands.num_token; i++) {
        printf("%s\n", commands.command_list[i]);
    }
    printf("\n");

    return 0;
}

int commandsTest() {

    // test ls
    listDir();
    printf("\n");

    // test pwd
    showCurrentDir();
    printf("\n");

    // test mkdir 
    char *dirName = "new_dir" ;
    makeDir(dirName);
    listDir();
    printf("\n");

    // test cd
    changeDir(dirName);
    showCurrentDir();
    printf("\n");

    // test cp
    copyFile("/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/input-2.txt", 
    "/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/new_dir");
    listDir();
    printf("\n");

    // test mv
    moveFile("/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/test_direct_1/test_text.txt",
    "/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/test_direct_2");

    // test rm
    changeDir("/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/test_direct_2");
    listDir();
    printf("\n");
    deleteFile("trash.txt");
    listDir();
    printf("\n");

    // test cat
    displayFile("/home/users/lbixby/lucas_bixby_homework/lucas_bixby_assignments/CS415/project_1/example-output-1.txt");
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) 
{
    // fisrt we must check what mode our program is started in 

    stringParseTest();

    commandsTest();

    return 0;
}