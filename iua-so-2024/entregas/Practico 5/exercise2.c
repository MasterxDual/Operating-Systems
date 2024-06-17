/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 5, Ejercicio 2
 Objetivo: Comunicarse entre procesos utilizando pipes sin nombre
 Fecha: 02/05/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char const *argv[]) {
    int fd[2]; //        +------+
	//pipe[1] (write) --> | pipe | --> pipe[0] (read)
    //                   +------+
    pid_t pid_son1, pid_son2;
    time_t now; //For obtaining time in seconds
    ssize_t bytes_written;

    //Creating pipe "without name" and verifying if happened an error creating it
    if(pipe(fd) < 0) {
        printf("Value of error: %d\n", errno);
		perror("Error creating pipe...\n");
		exit(EXIT_FAILURE);
    } 

    //Creating son 1
    pid_son1 = fork();

    //Verifying if happened an error forking son 1
    if(pid_son1 < 0) {
        printf("Value of error: %d\n", errno);
		perror("Error forking son 1\n");
		exit(EXIT_FAILURE);
    } else if(pid_son1 == 0) {
        time(&now); //Getting actual time in seconds (it's stored in variable now)
        char *str_time = ctime(&now); //Converts actual time in seconds to a readable string by humans
        
        size_t nbytes = strlen(str_time); //Quantity of bytes to write

        bytes_written = write(fd[1], str_time, nbytes); //Writing in pipe

        //Verifying if happened an error writing pipe with son 1
        if(bytes_written < 0) {
            printf("Error number %d\n", errno);
            perror("Can't write in pipe...\n");
            exit(EXIT_FAILURE);
        }

        //Printing message written by son process 1 in pipe
        printf("Pipe content written by process son 1: %s\n", str_time);
    } else {
        //Creating son 2
        pid_son2 = fork();

        //Verifying if happened an error forking son 2
        if(pid_son2 < 0) {
            printf("Value of error: %d\n", errno);
		    perror("Error forking son 2");
		    exit(EXIT_FAILURE);
        } else if(pid_son2 == 0) {
            char buf[50]; //I'm going to read only a string of the actual date in the format (Fri Sep 28 08:13:09 2018), so we can see that with less than 50 characters it will be okay
            ssize_t bytes_read;
            ssize_t nbytes = sizeof(buf); //We can't use strlen because buf it's empty before it's used in read function. 

            bytes_read = read(fd[0], buf, nbytes); //Reading content of pipe

            buf[bytes_read] = '\0';

            printf("Date read from pipe with process son 2: %s\n", buf);

            //Verifying if happened an error reading pipe from son 2
            if(bytes_read < 0) {
                printf("Error number %d\n", errno);
                perror("Can't read pipe from son 2\n");
                exit(EXIT_FAILURE);
            }

        } else {
            sleep(3);
            printf("I think my two sons did the work that they had to do, finishing program...\n");
            exit(EXIT_SUCCESS);
        }
    }


    return 0;
}
