/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 5, Ejercicio 3 programa a
 Objetivo: Comunicarse entre procesos utilizando pipes con nombre
 Fecha: 02/05/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char const *argv[]) {
    const char *fifo_name = "myfifo"; //name of fifo "with name"
    time_t now; //For obtaining time in seconds
    ssize_t bytes_written;


    //0666 = grants read and write permissions to all users
    if (mkfifo(fifo_name, 0666) == -1) {
        //EEXIST is created when fifo was created before using this function (mkfifo)
        //if (errno == EEXIST) --> Fifo "with name" already exists, so we don't do anything if happens
        if(errno != EEXIST) {
            printf("Error number %d\n", errno);
            perror("Error making fifo");
            exit(EXIT_FAILURE);
        }
    }

    //Open fifo for writing
    int fd = open(fifo_name, O_WRONLY);
    if (fd < 0) {
        printf("Error number %d\n", errno);
        perror("Error opening fifo\n");
        exit(EXIT_FAILURE);
    }
    
    time(&now); //Getting actual time in seconds (it's stored in variable now)
    char *str_time = ctime(&now); //Converts actual time in seconds to a readable string by humans
    size_t nbytes = strlen(str_time); //Quantity of bytes to write

    bytes_written = write(fd, str_time, nbytes); //Writing in pipe

    //Verifying if happened an error writing pipe with program (process) 1
    if(bytes_written < 0) {
        printf("Error number %d\n", errno);
        perror("Can't write in pipe...\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //Printing message written by process (program) 1 in pipe
    printf("Pipe content written by process (program) 1: %s\n", str_time);

    //Closing pipe file. The next program (program 2) will open and read this pipe created by program 1
    close(fd);

    return 0;
}
