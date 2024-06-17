/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 2, Ejercicio 2-b
 Objetivo: Sincronización de hilos
 Otra alternativa de solución, usando un arreglo global en vez de una estructura. Es mas simple ya que
 se observa que la estructura tiene datos de mas que no hacen falta declarar ni usar.
 Fecha: 28/03/2024
\**************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define NUM_THREADS 5
//Variable with error number if it occurs.
extern int errno;

//Global array to charge Fibonacci Serie.
int array[NUM_THREADS];

//Function that created thread's will use.
void *runner(void *param);

int main(int argc, char const *argv[])
{
    int rc;
    pthread_attr_t attr;
    pthread_t threads[NUM_THREADS];
    int index;
    
    //Creating five threads to show Fibonacci serie.
    for (int t = 0; t < NUM_THREADS; t++ ) {
        printf("*--------------------------*\nIn main: creating thread %d\n", t+1);
        pthread_attr_init(&attr);
        
        //Defining index variable of global array so I can walk through the arrangement (array) variable.
        index = t;

        //Creating thread by thread (there's inside an arrangement).
        rc = pthread_create(&threads[t], &attr, runner, (void *) &index);

        /*Waiting for next thread to be finished, later it will create another thread.
        Also it's checking if pthread_join has an error.*/
        if(pthread_join(threads[t], NULL) != 0) {
            printf("Error number %d\n", errno);
            perror("Program will finish:");
            exit(EXIT_FAILURE);
        }

        //Checking if pthread_create has an error.
        if(rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            printf("Error number %d\n", errno);
            perror("Program will finish:");
            exit(EXIT_FAILURE);
        }
    }

    //Waiting 3 seconds to print thread's id.
    sleep(3);

    printf("*--------------------------*\n");

    // Printing thread id again to check id's.
    for(int z = 0; z < NUM_THREADS; z++) {
        printf("Id de thread #%d -> %ld\n", z+1, threads[z]);
    }
    
    printf("*--------------------------*\n");

    //Printing Fibonacci Serie:
    for(int z = 0; z < NUM_THREADS; z++) {
        printf("Fibonacci Serie:\nArray[%d] = %d\n", z+1, array[z]);
    }

    //End main thread.
    pthread_exit(NULL);

    return 0;
}

void *runner(void *param) {
    //Casting (void *) to (int *). This will be a pointer to an array of type int.
    int *index = (int *) param;

    /*
    Array index of struct thread:  0 1 2 3 4 5 6 7  ...
    Array values of struct thread: 0 1 1 2 3 5 8 13 ...*/
    if(*index == 0) {
        array[0] = 0;
    } else if(*index == 1) {
        array[1] = 1;
    } else {
        //a_n = a_n-1 + a_n-2
        array[*index] = array[*index-1] + array[*index-2];
    }
    //Printing array value and he's corresponding index.
    printf("Hello World, I'm the thread %d.\nMy value is %d\n", *index+1, array[*index]);

    //Exiting actual thread created succesfully.
    pthread_exit(EXIT_SUCCESS);
}