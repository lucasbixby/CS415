#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void listDir()
/*for the ls command*/
{
    // open the current working directory
    DIR *dir = opendir(".");
    if( dir == NULL ){
        perror("listDir");
        return;
    }

    // print contents 
    struct dirent *entry ;
    while((entry = readdir(dir)) != NULL ) {
        write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
        write(STDOUT_FILENO, "\n", 1);
    }

    closedir(dir);
}

void showCurrentDir()
/*for the pwd command*/
{
    // initialize a char buffer 
    char buffer[1024];

    // check if 
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        perror("showCurrentDir");
        return;
    }

    write(STDOUT_FILENO, buffer, strlen(buffer));
    write(STDOUT_FILENO, "\n", 1);
}

void makeDir(char *dirName)
/*for the mkdir command*/
{
    if ( mkdir(dirName, 0755) == -1){
        perror("mkdir");
        return;
    }
}

void changeDir(char *dirName)
/*for the cd command*/
{
    return;
}

void copyFile(char *sourcePath, char *destinationPath) 
/*for the cp command*/
{
    return;
}

void moveFile(char *sourcePath, char *destinationPath)
/*for the mv command*/
{
    return;
}

void deleteFile(char *filename)
/*for the rm command*/
{
    return;
}

void displayFile(char *filename)
/*for the cat command*/
{
    return;
}