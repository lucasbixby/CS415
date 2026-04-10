#ifndef STRING_PARSER_H
#define STRING_PARSER_H


typedef struct
{
    char** command_list;    /* Array of individual commands separated by ';' */
    int num_token;          /* Number of tokens/commands found */
} command_line;


command_line str_tokenize(char* str);

/* Function to free the dynamic memory allocated within the command_line struct */
void free_command_line(command_line* control);

#endif