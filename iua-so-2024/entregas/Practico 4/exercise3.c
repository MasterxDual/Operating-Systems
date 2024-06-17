/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 4, Ejercicio 3
 Objetivo: Utilizando fork, funciones de escritura y de lectura de archivos juntos
 Fecha: 26/04/2024
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
    pid_t pid;
    pid_t ppid;
    char *data_path = "data2.txt";
    char character_to_write = '0';
    /*I think with a maximum of 2 chars length it will be enough to read the file.
    buf: buf needs to point to a valid memory location with a length not smaller than the specified size (nbytes) because of overflow.
    nbytes: is the requested number of bytes read, while the return value is the actual number of bytes read. Also, sometimes
    read system call should read fewer bytes than nbytes.*/
    int fd;
    int i = 0;
    char buf[2]; //I use two characters because I need the character \0 of the final string (char array).
    size_t nbytes = sizeof(buf);
    ssize_t bytes_read;
    
    pid = fork();
    if(pid < 0) {
        printf("Value of error: %d\n", errno);
		perror("Program will finish:");
		exit(EXIT_FAILURE);
    } else if(pid == 0) {
        int flags = O_WRONLY | O_TRUNC | O_CREAT; //Write, truncate (eliminates content) file and creates file if doesn't exist
        mode_t mode = S_IRUSR | S_IWUSR; //User has read and write permission

        ppid = getppid();
		printf("Hi, I'm the son process number %i, I'm going to write number %c...\n", getpid(), character_to_write);
        
        sleep(5); //Waiting some time to write file data2

        fd = open(data_path, flags, mode);

        //Checking if there's an error opening file
        if(fd < 0) {
            printf("Error number %d\n", errno);
            perror("Can't open the file");
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_written = write(fd, &character_to_write, sizeof(character_to_write));
        
        if(bytes_written < 0) {
            printf("Error number %d\n", errno);
            perror("Can't write the file");
            exit(EXIT_FAILURE);
        }

        printf("%c written succesfully by son's process...\n", character_to_write);
        close(fd);
    } else {
        int flags = O_RDONLY; //Read only
        mode_t mode = S_IRUSR; //User has read permission
        printf("Hi, I'm the father process number %i. I'm waiting that my son write...\n", getpid());
        
        do {
            /*I eliminated errors so program don't show them because I want that parent process just keep trying to open and read,
            there's no utility closing program is there's an error because son will write the number that parent waits.*/
            fd = open(data_path, flags, mode);
            
            bytes_read = read(fd, buf, nbytes);

            if((int) *buf != 49 && (int) *buf != 48) {
                printf("Text file doesn't has 0 or 1 in it, please write 0 or 1 to continue...\n");
                fflush(stdout);    
            }

            //if(*buf == 1) is another way
            if((int) *buf == 49) { //*buf = buf[1] (name of pointer is name of first element of an array)
                printf("Number 1 found in file. Will keep revising text file (revision n°%d)...\n", i+1);
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
        exit(EXIT_SUCCESS);
    }
    

    return 0;
}
