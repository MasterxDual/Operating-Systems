/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 5, Ejercicio 1
 Objetivo: Comunicarse entre procesos utilizando archivos externos
 Fecha: 29/04/2024
\**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <bits/waitflags.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char const *argv[]) {
    pid_t pid_son1, pid_son2, pid_son3, pid_son4;
    int status1, status2; //For reporting termination status of both sons
    char *data_path1 = "a-inout.txt";
    char *data_path2 = "b-inout.txt";
    int fd1, fd2;
    int flags = O_WRONLY | O_TRUNC | O_CREAT | O_RDONLY; //Read, write, truncate (eliminates content) file and creates file if doesn't exist
    mode_t mode = S_IWUSR | S_IRUSR; //User has write and read permission
    time_t now; //For obtaining time in seconds

    //Creating the two files
    fd1 = open(data_path1, flags, mode);
    fd2 = open(data_path2, flags, mode);

    //Creating son 1
    pid_son1 = fork();

    //Verifying if happened an error opening files a-inout and b-inout
    if(fd1 < 0) {
        printf("Error number %d\n", errno);
        perror("Can't open the file a-inout\n");
        exit(EXIT_FAILURE);
    }
    if(fd2 < 0) {
        printf("Error number %d\n", errno);
        perror("Can't open the file b-inout\n");
        exit(EXIT_FAILURE);
    }

    //Verifying if happened an error closing files a-inout and b-inout
    if(close(fd1) < 0) {
        printf("Error number %d\n", errno);
        perror("Can't close the file a-inout\n");
        exit(EXIT_FAILURE);
    }
    if(close(fd2) < 0) {
        printf("Error number %d\n", errno);
        perror("Can't close the file b-inout\n");
        exit(EXIT_FAILURE);
    }

    //Verifying if happened an error forking son 1
    if(pid_son1 < 0) {
        printf("Value of error: %d\n", errno);
		perror("Error forking son 1\n");
		exit(EXIT_FAILURE);
    } else if(pid_son1 == 0) {
        //Son 1
        fd1 = open(data_path1, O_RDONLY, S_IRUSR); //Read-only in file a-inout
        fd2 = open(data_path2, O_WRONLY, S_IWUSR); //Write-only in file b-inout
        
        //Verifying if happened an error opening files a-inout and b-inout
        if(fd1 < 0) {
            printf("Error number %d\n", errno);
            perror("Can't open the file a-inout with son 1\n");
            exit(EXIT_FAILURE);
        }
        if(fd2 < 0) {
            printf("Error number %d\n", errno);
            perror("Can't open the file b-inout with son 1\n");
            exit(EXIT_FAILURE);
        }
        
        time(&now); //Getting actual time in seconds (it's stored in variable now)
        char *str_time = ctime(&now); //Converts actual time in seconds to a readable string by humans


        /*sizeof: Returns the size in bytes of the data type or variable. If you apply sizeof to a pointer, you will get the size of the pointer, not 
        the size of the memory area it points to. Therefore, if you apply sizeof to a pointer, you will get the size of the pointer, not the size of the string it points to.
        strlen: Returns the length of a character string, that is, it counts all the characters until the null terminator ('\0') is found. This is what you need if you 
        want to get the length of a string of characters.*/
        ssize_t bytes_written;
        size_t nbytes = strlen(str_time); //Quantity of bytes to write

        bytes_written = write(fd2, str_time, nbytes); //Writing in b-inout

        //Verifying if happened an error writing file b-inout with son 1
        if(bytes_written < 0) {
            printf("Error number %d\n", errno);
            perror("Can't write the file b-inout with son 1...\n");
            exit(EXIT_FAILURE);
        }

        //Printing message written by son process 1 in file b-inout
        printf("File content written: %s\n", str_time);

        //Verifying if happened an error closing files a-inout and b-inout
        if(close(fd1) < 0) {
            printf("Error number %d\n", errno);
            perror("Can't close the file a-inout from son 1\n");
            exit(EXIT_FAILURE);
        }
        if(close(fd2) < 0) {
            printf("Error number %d\n", errno);
            perror("Can't close the file b-inout from son 1\n");
            exit(EXIT_FAILURE);
        }

    } else {
        //Creating son 2
        pid_son2 = fork();

        //Verifying if happened an error forking son 2
        if(pid_son2 < 0) {
            printf("Value of error: %d\n", errno);
		    perror("Error forking son 2");
		    exit(EXIT_FAILURE);
        } else if(pid_son2 == 0) {
            //THIS SOLVED THE PROBLEM OF NOT SHOWING WHAT SON 2 READ FROM FILE (ONE HOUR TRYING TO FIGURE OUT)
            sleep(5);

            //Son 2
            fd1 = open(data_path1, O_WRONLY, S_IWUSR); //Write-only in file a-inout
            fd2 = open(data_path2, O_RDONLY, S_IRUSR); //Read-only in file b-inout

            //Verifying if happened an error opening files a-inout and b-inout
            if(fd1 < 0) {
                printf("Error number %d\n", errno);
                perror("Can't open the file a-inout with son 2\n");
                exit(EXIT_FAILURE);
            }
            if(fd2 < 0) {
                printf("Error number %d\n", errno);
                perror("Can't open the file b-inout with son 2\n");
                exit(EXIT_FAILURE);
            }

            char buf[50]; //I'm going to read only a string of the actual date in the format (Fri Sep 28 08:13:09 2018), so we can see that with less than 50 characters it will be okay
            ssize_t bytes_read;
            ssize_t nbytes = sizeof(buf); //We can't use strlen because buf it's empty before it's used in read function. THIS SOLVED THE PROBLEM OF NOT SHOWING WHAT SON 2 READ FROM FILE

            bytes_read = read(fd2, buf, nbytes); //Reading file b-inout

            buf[bytes_read] = '\0';

            printf("Date read from process 2: %s\n", buf);

            //Verifying if happened an error closing files a-inout and b-inout
            if(close(fd1) < 0) {
                printf("Error number %d\n", errno);
                perror("Can't close the file a-inout from son 2\n");
                exit(EXIT_FAILURE);
            }
            if(close(fd2) < 0) {
                printf("Error number %d\n", errno);
                perror("Can't close the file b-inout from son 2\n");
                exit(EXIT_FAILURE);
            }

            //Verifying if happened an error reading file b-inout from son 2
            if(bytes_read < 0) {
                printf("Error number %d\n", errno);
                perror("Can't read the file b-inout from son 2\n");
                exit(EXIT_FAILURE);
            }

        } else {
            //Father
            //Here we are going to wait two son's process to finish, then father will print exit or termination status of both processes. 0 is option by default
            pid_son3 = waitpid(pid_son1, &status1, 0);
            pid_son4 = waitpid(pid_son2, &status2, 0);

            if(pid_son3 < 0) {
                printf("Error number %d\n", errno);
                perror("Ocurred an error waiting process son 1\n");
                exit(EXIT_FAILURE);
            }
            if(pid_son4 < 0) {
                printf("Error number %d\n", errno);
                perror("Ocurred an error waiting process son 2\n");
                exit(EXIT_FAILURE);
            }

            /*Here we analyze the value of the exit status of the child process. Below we detail what each macro does:
            WIFEXITED: returns true if the child process finished successfully.
            WEXITSTATUS: returns the exit state of the child.
            WIFSIGNALED: returns true if the child process was terminated by a signal.
            WTERMSIG: returns the number of the signal that caused the child process to terminate.
            WIFSTOPPED: returns true if the child process was stopped by delivering a signal.
            WSTOPSIG: returns the number of the signal that caused the child to stop.
            WIFCONTINUED: returns true if the child process was resumed by SIGCONT delivery */
            if (WIFEXITED(status1)) {
                printf("Exited, status: %d\n", WEXITSTATUS(status1));
            } else if (WIFSIGNALED(status1)) {
                psignal(WTERMSIG(status1), "Killed by signal:");
            } else if (WIFSTOPPED(status1)) {
                psignal(WSTOPSIG(status1), "Stopped by signal:");
            } else if (WIFCONTINUED(status1)) {
                printf("Continued process\n");
            }
            if (WIFEXITED(status2)) {
                printf("Exited, status: %d\n", WEXITSTATUS(status2));
            } else if (WIFSIGNALED(status2)) {
                psignal(WTERMSIG(status2), "Killed by signal:");
            } else if (WIFSTOPPED(status2)) {
                psignal(WSTOPSIG(status2), "Stopped by signal:");
            } else if (WIFCONTINUED(status2)) {
                printf("Continued process\n");
            }

            //Printing termination (exit) status of both processes
            printf("The exit status of my son's process 1 is %d.\n", status1);
            printf("The exit status of my son's process 2 is %d.\n", status2);

            exit(EXIT_SUCCESS);
        }
    }


    return 0;
}
