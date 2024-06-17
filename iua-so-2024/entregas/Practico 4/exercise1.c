/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 4, Ejercicio 1
 Objetivo: Verificando periódicamente la existencia de un archivo
 Fecha: 14/04/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char *data_path = "data";
    int flags = O_RDONLY | O_EXCL; //Read-only and prevent creation if it already exists.
    mode_t mode = S_IRUSR; //User has read permission.
    int fd;
    int i = 0;

    //If open gave us an error, (it wasn´t possible to read) the program will keep revising if exits the file to read.
    while(fd = open(data_path, flags, mode) < 0) {
        printf("Revising if there exists the file to read, revision number: (%d)\r", (i+1));
        i++;
        fflush(stdout);
        sleep(2);
    }

    //Checking if there was read the file succesfully.
    printf("File read succesfully. Finishing program...");
    close(fd);
    exit(EXIT_SUCCESS);

    return 0;
}

