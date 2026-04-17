#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
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

    free_command_line(&commands);
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

int runTests() {

    stringParseTest();
    commandsTest();

    return 0; 
}

char **parse_args(char* command_string, int *count) 
{
    // parse the whitespace from the command line to get elments 

    /*
    Dev note:

        make sure that this helper function handles strings that
    have length 0 ( no arguments ). 
    
    */

    int capacity = 10;
    char **args = malloc(capacity * sizeof(char *));
    if (args == NULL) {
        *count = 0;
        return NULL;
    }

    *count = 0; 

    char *token = strtok(command_string, " \t\n");
    while (token != NULL) {
        if (*count >= capacity) {
            capacity *= 2;
            char **temp = realloc(args, capacity * sizeof(char *));
            if (temp == NULL) {
                free(args);
                *count = 0;
                return NULL;
            }
            args = temp;
        }

        args[(*count)++] = token;
        token = strtok(NULL, " \t\n");
    }

    return args;
}

int fileMode(char *fileName) {
    // check to see if the provided file name is valid 

    /*
    Dev note:

        most of the implementation for fileMode is complete
    next step is to make the output write to the output.txt 
    file. right now, the file is opened but never used. 
    
    */

    FILE *input_file = fopen(fileName, "r");
    if (input_file == NULL) {
        perror("fopen input");
        return -1; 
    } 

    int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open output");
        fclose(input_file);
        return -1;
    }

    int saved_stdout = dup(STDOUT_FILENO);
    dup2(output_fd, STDOUT_FILENO);

    char *line = NULL;
    size_t len = 0; 
    ssize_t nread; 

    while ((nread = getline(&line, &len, input_file)) != -1) {
        if (nread > 0 && line[nread -1] == '\n') {
            line[nread -1] = '\0';
        }

        command_line cmd = str_tokenize(line);

        for (int i = 0; i < cmd.num_token; i++) {
            // fprintf(output_file, "command[%d]: %s\n", i, cmd.command_list[i]);
            
            int arg_count; 
            char **args = parse_args(cmd.command_list[i], &arg_count);

            if (arg_count == 0) {
                free(args);
                continue;
            }

            if( strcmp(args[0], "ls") == 0 ) {
                if (arg_count != 1) {
                    free(args);
                    continue;
                }
                listDir();

            } else if( strcmp(args[0], "pwd") == 0 ) {
                if (arg_count != 1) {
                    free(args);
                    continue;
                }
                showCurrentDir();

            } else if( strcmp(args[0], "mkdir") == 0 ) {
                if (arg_count != 2) {
                    free(args);
                    continue;
                }
                makeDir(args[1]);

            } else if( strcmp(args[0], "cd") == 0 ) {
                if (arg_count != 2) {
                    free(args);
                    continue;
                }
                changeDir(args[1]);

            } else if( strcmp(args[0], "cp") == 0 ) {
                if (arg_count != 3) {
                    free(args);
                    continue;
                }
                copyFile(args[1], args[2]);

            } else if( strcmp(args[0], "mv") == 0 ) {
                if (arg_count != 3) {
                    free(args);
                    continue;
                }
                moveFile(args[1], args[2]);

            } else if( strcmp(args[0], "rm") == 0 ) {
                if (arg_count != 2) {
                    free(args);
                    continue;
                }
                deleteFile(args[1]);

            } else if( strcmp(args[0], "cat") == 0 ) {
                if (arg_count != 2) {
                    free(args);
                    continue;
                }
                displayFile(args[1]);

            } else {
                // comamnd is invalid
                dprintf(STDOUT_FILENO, "Error! Unrecognized command: %s \n", args[0]);
            }

            free(args);
        }

        free_command_line(&cmd);
    }

    dprintf(STDOUT_FILENO, "End of file \n");
    dprintf(STDOUT_FILENO, "Bye Bye! \n");

    fflush(stdout);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    free(line);
    fclose(input_file);
    close(output_fd);

    return 0;
}

int interactiveMode() {

    return 0; 
}

int main(int argc, char *argv[]) 
{
    // run tests for comands and string_parser 
    /* runTests(); */

    // first we must check what mode our program has started in 
    if ( argc == 1 ) {
         interactiveMode();
    }
    else if ( argc == 3 ) {
        if ( strcmp(argv[1], "-f") != 0 ){
            fprintf(stderr, "Usage: ./psudo-shell -f <filename>\n");
            return -1; 
        }
        fileMode(argv[2]);
    } else {
        // there is an invalid amount of arguments 
        fprintf(stderr, "Usage: ./psudo-shell [-f filename]\n");
        return -1; 
    }

    return 0;
}