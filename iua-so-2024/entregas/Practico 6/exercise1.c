/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 6, Ejercicio 1
 Objetivo: Comprender semáforos sin nombre
 Fecha: 19/05/2024
\**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#define TRUE 1
#define BUFFER_SIZE 100 //This will determinate limit of the producer can produce

void *producer();
void *consumer();
void down(sem_t *sem);
void up(sem_t *sem);


typedef sem_t semaphore; /* semaphores are a special kind of int */
semaphore mutex; /* controls access to critical region */
semaphore empty; /* counts empty buffer slots */
semaphore full; /* counts full buffer slots */
int N = 0; //Shared value

/*There are three ways to do it:
 The first is to use thread variables so that they use the same global variable (which will be shared)
since they are thread processes, that is, they share the same memory.
 The second is to use fork (the father and his children will not use the same memory), therefore I will have
to print a message when producing or consuming since I will not be able to use the same global variable.
 The third is to use shared memory (it is more complex) with parent and child processes and then yes 
i will be able to use the same shared variable.*/

//Variable with error number if it occurs.
extern int errno;

int main(int argc, char const *argv[]) {
    pthread_t producer_thread, consumer_thread;
    pthread_attr_t attr;

    /*empty:
    -Represents the number of empty spaces in the buffer.
    -It is initialized with the size of the buffer (in this case BUFFER_SIZE) because at the beginning all the spaces are empty.
    -This allows the producer to produce until the buffer is full.
    full:
    -Represents the number of elements available to be consumed.
    -It is initialized to 0 because at startup there is no element produced.
    -This forces the consumer to wait until the producer has produced at least one item.
    mutex:
    -It is a binary semaphore that is used to ensure mutual exclusion in the critical section where the shared buffer is accessed.
    -It is initialized to 1 to allow the first thread (producer or consumer) to enter the critical section.*/

    //Initializing semaphores
    if(sem_init(&empty, 1, BUFFER_SIZE) != 0) { //Semaphore to initialize, 1 because it's shared between subprocesses, 100 it's initial value to produce up to 100 times.
        printf("Error number %d\n", errno);
        perror("Error initializing empty semaphore\n");
        exit(EXIT_FAILURE);
    } 
    if(sem_init(&full, 1, 0) != 0) { 
        printf("Error number %d\n", errno);
        perror("Error initializing full semaphore\n");
        exit(EXIT_FAILURE);
    }
    if(sem_init(&mutex, 1, 1) != 0) { 
        printf("Error number %d\n", errno);
        perror("Error initializing mutex semaphore\n");
        exit(EXIT_FAILURE);
    }

    //Initializing attributes values to default
    pthread_attr_init(&attr);

    //Creating threads producer and consumer
    if(pthread_create(&producer_thread, &attr, producer, NULL) != 0) {
        printf("Error number %d\n", errno);
        perror("Error initializing producer thread\n");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_create(&consumer_thread, &attr, consumer, NULL) != 0) {
        printf("Error number %d\n", errno);
        perror("Error initializing consumer thread\n");
        exit(EXIT_FAILURE);
    }

    //Wait threads to finish
    if(pthread_join(producer_thread, NULL) != 0) {
        printf("Error number %d\n", errno);
        perror("Error waiting producer thread\n");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_join(consumer_thread, NULL) != 0) {
        printf("Error number %d\n", errno);
        perror("Error waiting consumer thread\n");
        exit(EXIT_FAILURE);
    }

    //Destroying semaphores after using them
    if(sem_destroy(&empty) != 0) {
        printf("Error number %d\n", errno);
        perror("Error destroying empty semaphore\n");
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(&full) != 0) {
        printf("Error number %d\n", errno);
        perror("Error destroying full semaphore\n");
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(&mutex) != 0) {
        printf("Error number %d\n", errno);
        perror("Error destroying mutex semaphore\n");
        exit(EXIT_FAILURE);
    }

    printf("Main thread finished...\n%d items left produced...\nProgram will finish...", N);
    pthread_exit(NULL);

    return 0;
}

/*sem_wait and sem_post are functions of a library to manage POSIX semaphores. So this two functions will fix the problem
of managing the logic of the three semaphores for this producer-consumer problem*/
void down(sem_t *sem) {
    sem_wait(sem); //Decrementing semaphore
}

void up(sem_t *sem) {
    sem_post(sem); //Incrementing semaphore
}

void *producer(void) {
    int count = 0;
	while (count != 8) { /* TRUE is the constant 1 */
		down(&empty); /* decrement empty count */
		down(&mutex); /* enter critical region */
		N++;
        printf("----------------------------\nIteration of producer: %d\nProduced: N = %d\n", count+1, N);
		up(&mutex); /* leave critical region */
		up(&full); /* increment count of full slots */
        count++;
        sleep(2);
	}
    printf("Producer thread finished...\n");
    pthread_exit(NULL);

    return NULL;
}

void *consumer(void) {
    int count = 0;
	while (count != 6) { 
		down(&full); /* decrement full count */
		down(&mutex); /* enter critical region */
		N--;
        printf("----------------------------\nIteration of consumer: %d\nConsumed: N = %d\n", count+1, N);
		up(&mutex); /* leave critical region */
		up(&empty); /* increment count of empty slots */
        count++;
		sleep(6);
	}
    printf("Consumer thread finished...\n");
    pthread_exit(NULL);

    return NULL;
}