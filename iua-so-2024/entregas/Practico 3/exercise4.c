/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 3, Ejercicio 4
 Objetivo: Copiando un archivo pasado por linea de comando a otro archivo destino.
 Fecha: 15/04/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char *argv[]) {
    /*argv[0] has the name of the program executed in shell: ./my_program
    For example: if we write the following command in shell: ./my_program hello there, array argv will be:
    argv[0] = ./my_program
    argv[1] = hello
    argv[2] = there
    We need to use argv[1] to receive the name of the file to be read. So WE CAN USE ANY WORD OF THE NEW FILE TO BE CREATED AND WRITTEN.
    */
    char *data_path = argv[1];
    int flags1 = O_RDONLY | O_EXCL; //Read-only and prevent creation if it already exists.
    mode_t mode1 = S_IRUSR; //User has read permission.

    int flags2 = O_RDWR|O_CREAT; //Read-write, and create if it doen't exist the file to be opened. 
    /*S_IRUSR: Read permission for the file owner. 
    S_IWUSR: Write permission bit for the owner of the file
    S_IRGRP: Read permission bit for the group owner of the file.
    S_IROTH: Read permission for users other than the file owner.
    */
    mode_t mode2 = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH; 


    //Opening origin file to copy it's content to destination file.
    int fd1 = open(data_path, flags1, mode1);

    //Opening destination file to paste the content of origin file in it.
    int fd2 = open("data2.bak", flags2, mode2);

    /*I counted total characters in txt data_path file and it returned me 608 characters. So i will create a buffer greater
    than that length.
    buf: buf needs to point to a valid memory location with a length not smaller than the specified size (nbytes) because of overflow.
    nbytes: is the requested number of bytes read, while the return value is the actual number of bytes read. Also, sometimes
    read system call should read fewer bytes than nbytes.*/
    char buf[700];
    size_t nbytes = sizeof(buf);
    ssize_t bytes_read;
    ssize_t bytes_wrote;

    //Checking if there's an error opening origin file or destination file
    if(fd1 < 0 || fd2 < 0) {
        printf("Error number %d\n", errno);
        perror("Program will finish.");
        exit(EXIT_FAILURE);
    }

    //Iterating through origin_file, if return value of read is not equal 0, still there's reading origin_file
    while((bytes_read = read(fd1, buf, nbytes)) > 0) {
        bytes_wrote = write(fd2, buf, bytes_read);
        printf("File content read, that will be written in the destination file:\n");
        printf("%s\n", buf); //I print whole content that has char array buffer that saved content of origin_file.
        printf("Reading file: bytes_read: %ld\n", bytes_read); //Testing return variable of read function.
    }
    
    //Checking if there's an error reading file
    if(bytes_read == -1) {
        printf("Error number %d\n", errno);
        perror("Can't read the destination file.");
        exit(EXIT_FAILURE);
    }

    //Checking if there's an error writing destination file
    if(bytes_wrote < 0) {
            printf("Error number %d\n", errno);
            perror("Can't write the destination file.");
            exit(EXIT_FAILURE);
    }

    //If return value of read is equal 0, it finished reading all file (it reached the end of the file)
    if(bytes_read == 0) {
        //Checking if there's an error closing origin file
        if(close(fd1) < 0) {
            printf("Error number %d\n", errno);
            perror("Program will finish.");
            exit(EXIT_FAILURE);
        }
        //Checking if there's an error closing destination file
        if(close(fd2) < 0) {
            printf("Error number %d\n", errno);
            perror("Program will finish.");
            exit(EXIT_FAILURE);
        }
        //Testing return variable of read and writefunction.
        printf("Finishing reading origin file and writing destination file: bytes_read: %ld bytes_wrote: %ld\n", bytes_read, bytes_wrote); 

        printf("Origin file and destination file closed succesfully.");
        exit(EXIT_SUCCESS);
    }


    return 0;
}

