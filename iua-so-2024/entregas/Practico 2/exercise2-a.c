/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 2, Ejercicio 2-a
 Objetivo: Sincronización de hilos
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

//Creating global struct variable
struct thread {
    pthread_t threads[NUM_THREADS];
    int array[NUM_THREADS];
    int index;
};

//Function that created thread's will use.
void *runner(void *param);

int main(int argc, char const *argv[])
{
    struct thread thread;
    int rc;
    pthread_attr_t attr;
    
    //Creating five threads to show Fibonacci serie.
    for (int t = 0; t < NUM_THREADS; t++ ) {
        printf("*--------------------------*\nIn main: creating thread %d\n", t+1);
        pthread_attr_init(&attr);
        
        //Defining index variable of struct thread so I can walk through the arrangement (array) variable of the struct.
        thread.index = t;

        //Creating thread by thread (there's inside an arrangement).
        rc = pthread_create(&thread.threads[t], &attr, runner, (void *) &thread);

        /*Waiting for next thread to be finished, later it will create another thread.
        Also it's checking if pthread_join has an error.*/
        if(pthread_join(thread.threads[t], NULL) != 0) {
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
        printf("Id de thread #%d -> %ld\n", z+1, thread.threads[z]);
    }

    printf("*--------------------------*\n");
    
    //Printing Fibonacci Serie:
    for(int z = 0; z < NUM_THREADS; z++) {
        printf("Fibonacci Serie:\nArray[%d] = %d\n", z+1, thread.array[z]);
    }

    //End main thread.
    pthread_exit(NULL);

    return 0;
}

void *runner(void *param) {
    /* Casting type void to (struct thread *)
    Step by reference:*/
    struct thread *thread_data = (struct thread *)param;

    /*Step by value:
    struct thread thread_data = *(struct thread *)param; 
    We can't do it step by value becase we need the original values of the array to be passed to main, and then it is neccesary modify it 
    to runner again, then transfer it to main, and so on. We could use step by value if we use the return value, 
    but in this case we cannot use it, another option is to use step by value if we need to operate with that value only in a block of code, that is locally.*/

    /*
    Array index of struct thread:  0 1 2 3 4 5 6 7  ...
    Array values of struct thread: 0 1 1 2 3 5 8 13 ...
    Step by reference:*/
    if(thread_data->index == 0) {
        thread_data->array[0] = 0;
    } else if(thread_data->index == 1) {
        thread_data->array[1] = 1;
    } else {
        //a_n = a_n-1 + a_n-2
        thread_data->array[thread_data->index] = thread_data->array[thread_data->index - 1] + thread_data->array[thread_data->index - 2];
    }

    /*Step by value: 
    if(thread_data.index == 0) {
        thread_data.array[0] = 0;
    } else if(thread_data.index == 1) {
        thread_data.array[1] = 1;
    } else {
        //a_n = a_n-1 + a_n-2
        thread_data.array[thread_data.index] = thread_data.array[(thread_data.index)-1] + thread_data.array[(thread_data.index)-2];
    } */
    /*Printing array value and he's corresponding index, also thread's id.
    Step by reference:*/
    printf("Hello World, I'm the thread %d.\nMy value is %d\nAnd my id is %ld\n", thread_data->index+1, thread_data->array[thread_data->index], thread_data->threads[thread_data->index]);

    /* Step by value:
    printf("Hello World, I'm the thread %d.\nMy value is %d\nAnd my id is %ld\n", thread_data.index+1, thread_data.array[thread_data.index], thread_data.threads[thread_data.index]); */

    //Exiting actual thread created succesfully.
    pthread_exit(EXIT_SUCCESS);
}