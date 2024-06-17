/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 4, Ejercicio 4
 Objetivo: Intercambio de mensajes entre dos procesos
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

extern int errno;

int main(int argc, char *argv[]) {
    pid_t pid;
    char *data_path = "comunicador.txt";
    int fd;
    int i = 0, j = 0;
    char buf[100];
    size_t nbytes = sizeof(buf);
    ssize_t bytes_read;
    char last_message[100];
    
    pid = fork();
    if(pid < 0) {
        printf("Value of error: %d\n", errno);
		perror("Program will finish:");
		exit(EXIT_FAILURE);
    } else if(pid == 0) {
        char *message = "Hi, I'm writing in file (Son A)";
        int flags = O_WRONLY | O_TRUNC | O_CREAT; //Write, truncate (eliminates content) file and creates file if doesn't exist
        mode_t mode = S_IRUSR | S_IWUSR; //User has read and write permission
        int first = 1;
        strcpy(buf, message);

		printf("Hi, I'm writing something (Son A)...\n");
        fflush(stdout);

        fd = open(data_path, flags, mode);

        //Checking if there's an error opening file
        if(fd < 0) {
            printf("Error number %d\n", errno);
            perror("Can't open the file");
            exit(EXIT_FAILURE);
        }

        //Son process writing in text file
        ssize_t bytes_written = write(fd, buf, nbytes);
        
        if(bytes_written < 0) {
            printf("Error number %d\n", errno);
            perror("Can't write the file\n");
            exit(EXIT_FAILURE);
        }

        //Printing message written by son process
        printf("File content written:\n%s\n", buf);

        flags = O_RDONLY; //Read only
        mode = S_IRUSR; //User has read permission

        //Attempting to read periodically file until text content changes
        do {
            sleep(2);
            printf("(Son A) Reading periodically message n°%d\n", ++j);
            fflush(stdout);

            fd = open(data_path, flags, mode);
            
            bytes_read = read(fd, buf, nbytes);
            buf[bytes_read] = '\0';


            //If both messages are different, it will finish loop
            if(strcmp(buf, last_message) != 0 && !first) {
                break;
            }

            //Copying text message to a variable to compare this with the next message written by parent process
            strcpy(last_message, buf);

            close(fd);
            fflush(stdout);
            first = 0;
        } while(1); //If content of text read is equal to last message it will keep doing loop

        fflush(stdout);
        printf("\nSon process (A) will finish...\n");            
        exit(EXIT_SUCCESS);
    } else {
        int flags = O_RDWR | O_TRUNC; //Write-read and truncate (eliminates content) file
        mode_t mode = S_IRUSR | S_IWUSR; //User has read and write permission
        char message_to_write[100] = "Hello, I'm writing something different (Parent B)...";

        printf("Hi, I'm the father process (B)\n");
        sleep(10);
        
        /*Attempting to look if text file exists, if exists then it will overwrite the last message to a different message so
        son process will revise it and then finish because actual message it's different to last message*/
        do {
            printf("(Parent B) Looking for text file...\n");
            fd = open(data_path, flags, mode);         
            
            bytes_read = read(fd, buf, nbytes);

            ssize_t bytes_written = write(fd, message_to_write, strlen(message_to_write)); //strlen returns the actual length of the message without counting the trailing null character.
            

            if(bytes_written < 0) {
                printf("Error number %d\n", errno);
                perror("Can't write the file");
                exit(EXIT_FAILURE);
            }

            close(fd);
            fflush(stdout);
        } while(fd < 0); //If it can't open file, it will revising if text file exists


        printf("\nParent has written succesfully\n");
        //Printing message written by parent process
        printf("File content written:\n%s\n", message_to_write);
        printf("Parent (B) will end...\n");
    }
    

    return 0;
}
