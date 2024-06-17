/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 3, Ejercicio 2
 Objetivo: Imprimir contenido de un archivo creado con comandos de salida estándar
 Fecha: 14/04/2024
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
    char *data_path = "data";
    int flags = O_RDONLY | O_EXCL; //Read-only and prevent creation if it already exists.
    mode_t mode = S_IRUSR; //User has read permission.
    int fd = open(data_path, flags, mode);

    /*I counted total characters in txt data_path file and it returned me 1259 characters. So i will create a buffer greater
    than that length.
    buf: buf needs to point to a valid memory location with a length not smaller than the specified size (nbytes) because of overflow.
    nbytes: is the requested number of bytes read, while the return value is the actual number of bytes read. Also, sometimes
    read system call should read fewer bytes than nbytes.*/
    char buf[1300];
    size_t nbytes = sizeof(buf);
    ssize_t bytes_read;
    
    //Checking if there's an error opening file
    if(fd < 0) {
        printf("Error number %d\n", errno);
        perror("Can't open the file");
        exit(EXIT_FAILURE);
    }

    //Iterating through data file, if return value of read is not equal 0, still there's reading data file
    while((bytes_read = read(fd, buf, nbytes)) > 0) {
        printf("File content:\n");
        printf("%s\n", buf); //I print whole content that has char array buffer that saved content of data file.
        printf("Reading file: bytes_read: %ld\n", bytes_read); //Testing return variable of read function.
    }

    //Checking if there's an error reading file        
    if(bytes_read < 0) {
        printf("Error number %d\n", errno);
        perror("Can't read file.");
        exit(EXIT_FAILURE);
    }

    //If return value of read is equal 0, it finished reading all file (it reached the end of the file)
    if(bytes_read == 0) {
        //Checking if there's an error closing file
        if(close(fd) < 0) {
            printf("Error number %d\n", errno);
            perror("Can't close the file.");
            exit(EXIT_FAILURE);
        }
        printf("Finishing reading file: bytes_read: %ld\n", bytes_read); //Testing return variable of read function.
        printf("File closed succesfully.");
        exit(EXIT_SUCCESS);
    }

    return 0;
}

