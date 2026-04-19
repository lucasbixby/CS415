/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/

	if ( buf == NULL) {
		perror("NULL string");
		return -1; 
	}
	int count = 0; 
	int i = 0;

    while (buf[i] != '\0') {
		// string starts/ends with delimeter 
        while (buf[i] != '\0' && strchr(delim, buf[i]) != NULL) {
            i++;
        }

		// account NULL for the last token 
        if (buf[i] != '\0') {
            count++;
        }

		// skip the token 
        while (buf[i] != '\0' && strchr(delim, buf[i]) == NULL) {
            i++;
        }
    }

    return count;
}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/

	command_line command;
    command.num_token = 0;
    command.command_list = NULL;

    if (buf == NULL) {
        return command;
    }

    // remove trailing newline if present 
    char *saveptr;
    strtok_r(buf, "\n", &saveptr);

    // count tokens
    command.num_token = count_token(buf, delim);

    // allocate array of char* 
    command.command_list = malloc(sizeof(char*) * (command.num_token + 1));
    if (command.command_list == NULL) {
        command.num_token = 0;
        return command;
    }

    // fill command_list
    int i = 0;
    char *saveptr2;
    char *token = strtok_r(buf, delim, &saveptr2);

    while (token != NULL) {
        command.command_list[i] = malloc(strlen(token) + 1);
        if (command.command_list[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(command.command_list[j]);
            }
            free(command.command_list);
            command.command_list = NULL;
            command.num_token = 0;
            return command;
        }

        strcpy(command.command_list[i], token);
        i++;
        token = strtok_r(NULL, delim, &saveptr2);
    }

    // fill last spot with NULL
    command.command_list[i] = NULL;

    return command;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/

	if (command == NULL || command->command_list == NULL) {
        return;
    }

    for (int i = 0; i < command->num_token; i++) {
        free(command->command_list[i]);
    }

    free(command->command_list);
    command->command_list = NULL;
    command->num_token = 0;
}
