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

void lfcat()
{
/* High level functionality you need to implement: */
	
	/* Get the current directory with getcwd() */
	char buffer[1024];

    // check if 
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        perror("showCurrentDir");
        return;
    }
	
	/* Open the dir using opendir() */
	DIR *dir = opendir(buffer);
    if( dir == NULL ){
        perror("openDir");
        return;
    }

    // print contents 
	
	/* use a while loop to read the dir with readdir()*/

	struct dirent *entry ;
	while((entry = readdir(dir)) != NULL ) {
		/* You can debug by printing out the filenames here */
    
		/* Option: use an if statement to skip any names that are not readable files (e.g. ".", "..", "main.c", "lab2.exe", "output.txt" */
		if (strcmp(entry->d_name, ".") == 0 ||
			strcmp(entry->d_name, "..") == 0) {
        	continue;
    	}
		
		char *ext = strrchr(entry->d_name, '.');
		if (ext && (strcmp(ext, ".o") == 0 ||
		 			strcmp(ext, ".exe") == 0 || 
					strcmp(ext, ".pdf") == 0 || 
					strcmp(ext, ".h") == 0 ||
					strcmp(ext, ".c") == 0)) {
    		continue;
		} 
		if( strcmp(entry->d_name, "output.txt") == 0 || 
			strcmp(entry->d_name, "Makefile") == 0 ){
			continue; 
		} 
		
		/* Open the file */
		FILE *fp = fopen(entry->d_name, "r");
		if (fp == NULL){
        	continue;
    	}
		
		write(STDOUT_FILENO, "File: ", 6);
    	write(STDOUT_FILENO, entry->d_name, strlen(entry->d_name));
    	write(STDOUT_FILENO, "\n", 1);
		

    	char *line = NULL;
    	size_t len = 0;
		ssize_t bytesRead;

		/* Read in each line using getline() */
    	while ((bytesRead = getline(&line, &len, fp)) != -1){
			/* Write the line to stdout */
    		write(STDOUT_FILENO, line, bytesRead);
    	}
		write(STDOUT_FILENO, "\n", 1);

		/* close the read file and free/null assign your line buffer */
		free(line);
    	fclose(fp);
			
		/* write 80 "-" characters to stdout */
		write(STDOUT_FILENO, "-------------------------------------------------------------------------------\n", 80);
	}
	/*close the directory you were reading from using closedir() */
	closedir(dir);
}
