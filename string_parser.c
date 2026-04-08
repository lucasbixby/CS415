#include "string_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// static function allows us to 
static char* trim_whitespace(char* s)
// formats the string by removeing leading and trailing whitespace.
{
    char* end;
    if (s == NULL){
        return NULL;
    }

    while (*s != '\0' && isspace((unsigned char)*s)) {
        s++;
    }

    if (*s == '\0') {
        return s;
    }

    end = s + strlen(s) - 1;

    while (end > s && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    // return the cleaned string.
    return s;

}

command_line str_tokenize(char* str)
// splits individual commands seperated by a ";" into a dynamically allocated array 
// and records how many commands were found. 
{
    command_line result;
    char* buffer; 
    char* token;
    int count = 0; 
    int i = 0;

    // initialize member variables of result.
    result.command_list = NULL;
    result.num_token = 0;

    if (str == NULL) {
        return result;
    }

    // initialize a buffer the size of the string length.
    buffer = malloc(strlen(str) + 1);
    if (buffer == NULL){
        return result;
    }
    strcpy(buffer, str); // copy string into the buffer.

    // seperates each line of commands by the delimiter ";", then trims leading/trailing 
    // whitespace to find if there are any empty commands. then counts the number of commands.
    token = strtok(buffer, ";");
    while (token != NULL) {
        char* trimmed = trim_whitespace(token);
        if (*trimmed != '\0') {
            count++;
        }
        token = strtok(NULL, ";"); // continues where parse pointer left of 
    }

    free(buffer);

    if(count == 0){
        return result;
    }

    // allocate spcae to the array based on the count of commands 
    result.command_list = malloc(count * sizeof(char*));
    if (result.command_list == NULL) {
        result.num_token = 0;
        return result;
    }
    result.num_token = count;

    // initialize buffer for input command string 
    buffer = malloc(strlen(str) + 1);
    if (buffer == NULL) {
        free(result.command_list);
        result.command_list = NULL;
        result.num_token = 0;
        return result;
    }
    strcpy(buffer, str);

    // parse buffer by ";"
    token = strtok(buffer, ";");
    while (token != NULL) {
        // clean-up individual command strings 
        char* trimmed = trim_whitespace(token);
        
        if (*trimmed != '\0') {
            // add command strings to the results command_list array 
            result.command_list[i] = malloc(strlen(trimmed) + 1);
            if (result.command_list[i] == NULL) {
                int j; 
                for (j = 0; j < i; j++) {
                    free(result.command_list[j]);
                }
                free(result.command_list);
                free(buffer);

                result.command_list = NULL;
                result.num_token = 0;
                return result;
            }

            strcpy(result.command_list[i], trimmed);
            i++;
        }
        // increment to the next command string
        token = strtok(NULL, ";");
    }    
    free(buffer);
    return result;
}

void free_command_line(command_line* control)
// helper function to memory allocated in str_tokenize() to prevent memory leaks
{
    int i;

    if (control == NULL) {
        return;
    }

    if (control->command_list != NULL) {
        for (i = 0; i < control->num_token; i++) {
            free(control->command_list[i]);
        }
        free(control->command_list);
    }

    control->command_list = NULL;
    control->num_token = 0;
}