/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 5, Ejercicio 3 programa b
 Objetivo: Comunicarse entre procesos utilizando pipes con nombre
 Fecha: 02/05/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char const *argv[]) {
    const char *fifo_name = "myfifo"; //name of fifo "with name"
    ssize_t bytes_read;
    char buf[50];
    ssize_t nbytes = sizeof(buf);

    //Open fifo for reading
    int fd = open(fifo_name, O_RDONLY);
    if (fd < 0) {
        printf("Error number %d\n", errno);
        perror("Error opening fifo\n");
        exit(EXIT_FAILURE);
    }

    bytes_read = read(fd, buf, nbytes);

    //Verifying if happened an error reading pipe with program (process) 2.
    if(bytes_read < 0) {
        printf("Error number %d\n", errno);
        perror("Can't read pipe...\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //Add null terminator to the end of the message to print it correctly
    buf[bytes_read] = '\0';

    //Printing message read by process (program) 2 in pipe.
    printf("Date read from pipe with process (program) 2: %s\n", buf);

    //Closing pipe file.
    close(fd);

    //Verifies if exists an error eliminating fifo "with name" from files.
    if(unlink(fifo_name) < 0) {
        printf("Error number %d\n", errno);
        perror("Error eliminating fifo\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
