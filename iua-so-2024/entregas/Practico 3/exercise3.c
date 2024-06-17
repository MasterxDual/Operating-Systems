/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 3, Ejercicio 3
 Objetivo: Aprender a utilizar opendir, readdir y closedir
 Fecha: 14/04/2024
\**************************************************************/

/*DT_REG: This is a regular file.
DT_DIR: This is a directory.*/
#define _DEFAULT_SOURCE //Now I can use DT_REG and DT_DIR
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char *argv[]) {
    //Init pointer to dir_ptr
    DIR *dir_ptr;

    //Init struct hold the returned value of readdir function
    struct dirent *read_dir;

    //Open the directory
    dir_ptr = opendir("/");

    //Check if there's an error while opening root directory
    if(dir_ptr == NULL) {
        printf("Value error: %d\n", errno);
        perror("Can't open the directory.\n");
        exit(EXIT_FAILURE);
    }

    //Make a loop to read whole entry inside the directory and check if there's an error with opening
    while((read_dir = readdir(dir_ptr)) != NULL) {
        //Check if there's any regular file, if there prints their name
        if(read_dir->d_type == DT_REG) {
            printf("FILE: %s\n", read_dir->d_name);
        } else if(read_dir->d_type == DT_DIR) { //Check if there are any subdirectories, and also access the structure and print their name
            printf("DIR: %s\n", read_dir->d_name);
        }
    }

    //Check if there's an error while closing root directory
    if(closedir(dir_ptr) < 0) {
        printf("Value error: %d\n", errno);
        perror("Can't close the directory.\n");
        exit(EXIT_FAILURE);
    } 
    printf("Directory closed succesfully.\n");
    exit(EXIT_SUCCESS);
    
    return 0;
}
