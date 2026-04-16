#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
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
    if ( chdir( dirName) == -1){
        perror("changeDir");
        return;
    }
}

void copyFile(char *sourcePath, char *destinationPath) 
/*for the cp command*/
{
    // open source file
    int source = open(sourcePath, O_RDONLY);
    if ( source == -1){
        perror("open source");
        return;
    }

    char *fileName = basename(sourcePath);
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", destinationPath, fileName);

    int dst = open(fullPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst == -1) {
        perror("open destinaiton");
        close(source);
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(source, buffer, sizeof(buffer))) > 0) {
        if (write(dst, buffer, bytesRead) != bytesRead) {
            perror("write");
            close(source);
            close(dst);
            return;
        }
    }

    if (bytesRead == -1) {
        perror("read");
    }

    close(source);
    close(dst);
}

void moveFile(char *sourcePath, char *destinationPath)
/*for the mv command*/
{
    copyFile(sourcePath, destinationPath);

    if (remove(sourcePath) == -1) {
        perror("remove");
        return;
    }
}

void deleteFile(char *filename)
/*for the rm command*/
{
    remove(filename);
}

void displayFile(char *filename)
/*for the cat command*/
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1){
        perror("open");
        return;
    }
    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){
        write(STDOUT_FILENO, buffer, bytesRead);
    }
    write(STDOUT_FILENO, "\n", 1);

    if (bytesRead == -1){
        perror("read");
    }
}