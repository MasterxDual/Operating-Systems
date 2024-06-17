/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 4, Ejercicio 2
 Objetivo: Verificando periódicamente la existencia de un archivo y leyendo su contenido.
 Fecha: 14/04/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

//Variable with error number if it occurs.
extern int errno;

/*
Referencia a if((int) *buf == 49) y demás:
    char a = '0';
    printf("Valor en ASCII: %d\n", (int) a);
    Imprimirá el valor numérico 48.
*/

int main(int argc, char *argv[]) {
    char *data_path = "data2";
    int flags = O_RDWR; //Read-write 
    mode_t mode = S_IRUSR | S_IWUSR; //User has read and write permission
    

    /*I think with a maximum of 2 chars length it will be enough to read the file.
    buf: buf needs to point to a valid memory location with a length not smaller than the specified size (nbytes) because of overflow.
    nbytes: is the requested number of bytes read, while the return value is the actual number of bytes read. Also, sometimes
    read system call should read fewer bytes than nbytes.*/
    int fd;
    int i = 0;
    char buf[2]; //I use two characters because I need the character \0 of the final string (char array).
    size_t nbytes = sizeof(buf);
    ssize_t bytes_read;

    do {
        
        fd = open(data_path, flags, mode);

        //Checking if there's an error opening file
        if(fd < 0) {
            printf("Error number %d\n", errno);
            perror("Can't open the file");
            exit(EXIT_FAILURE);
        }

        bytes_read = read(fd, buf, nbytes);

        //Checking if there's an error reading file        
        if(bytes_read < 0) {
            printf("Error number %d\n", errno);
            perror("Can't read file.");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if((int) *buf != 49 && (int) *buf != 48) {
            printf("Text file doesn't has 0 or 1 in it, please write 0 or 1 to continue...\r");
            fflush(stdout);    
        }

        //if(*buf == 1) is another way
        if((int) *buf == 49) { //*buf = buf[1] (name of pointer is name of first element of an array)
            printf("Number 1 found in file. Will keep revising text file (revision n°%d)...\r", i+1);
            fflush(stdout);
            i++;
        } 

        //¿Por qué funciona con esta línea de código?
        buf[bytes_read] = '\0'; 

        close(fd);
        sleep(2);
    } while((int) *buf != 48); //If content of text read is equal 1, it will keep doing the while

    printf("\nNumber 0 found in file.\nClosing program...");
    close(fd); 
    
    /* Colour tip:
    function atoi converts the initial portion of the string pointed to by nptr to int.
    It helps me when I want to compare a string of chars (string). In this case I didn't need it
    because we have to compare a single character
    */

    return 0;
}
