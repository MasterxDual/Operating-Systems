/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 1, Ejercicio 7
 Objetivo: Capturando señales y usando fork
 Fecha: 20/03/2024
\**************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

void handle_sigint(int sig_num);
void handle_sigtstp(int sig_num);

extern int errno;
int signalAction = 0;

int main(int argc, char const *argv[])
{
    
    pid_t pid = fork();

    /*Capturamos el error por si no se pudo hacer el fork().
    El proceso padre se encarga de verificar si las señales no dan error, es decir, se pudieron enviar y recibir sin errores,
    en el caso que no hayan aparecido errores en el envío de señales, se envía otra señal al proceso hijo en el caso que se 
    pulse CTRL+C o se pulse CTRL+Z. Luego estas dos pulsaciones se manejan en las funciones declaradas y definidas
    al final del programa.*/
    if(pid < 0) {
        printf("Value of error: %d\n", errno);
		perror("Program will finish:");
		exit(EXIT_FAILURE);
    } else if(pid > 0) {
        printf("Hi, I'm the father process, my id is %i.\nI'm going to send a signal to my son...\n", getppid());
        
        if(signal(SIGINT, handle_sigint) != SIG_ERR) {
            kill(pid, SIGINT);
        } else {
            printf("Error number:%d\n", errno);
            perror("Error setting signal handler");
        }

        if(signal(SIGTSTP, handle_sigtstp) != SIG_ERR) {
            kill(pid, SIGTSTP);
        } else {
            printf("Error number:%d\n", errno);
            perror("Error setting signal handler");
        }

        while(1) {
            if(signalAction == 1) {
                printf("Signal succesfully received.\n");
                signalAction = 0;
            }
        }
    }

    exit(EXIT_SUCCESS);
    
    return 0;
}

/**Captura la señal CTRL+C imprimiendo en pantalla el id del proceso hijo (el que está recibiendo la señal).
 * Luego imprime el valor de la señal capturada y establece el valor de la variable global a 1.
 * sig_num: código de la señal capturada.
*/
void handle_sigint(int sig_num) {
    if(sig_num == SIGINT) {
        printf("\nHi, I'm the son process, my id is: %i\n", getpid());
        printf("Caught signal %d\n", sig_num);
        signalAction = 1;
    }

    fflush(stdout);
}

/**Capturo la señal CTRL+Z imprimiendo en pantalla un mensaje describiendo lo que sucede mas la señal recibida,
 * luego este proceso hijo que capturó la señal envía otra señal al proceso padre para que se detenga, para finalmente
 * poder finalizar el programa por ende finalizar al proceso hijo.
 * sig_num: código de la señal capturada. 
*/
void handle_sigtstp(int sig_num) {
    if(sig_num == SIGTSTP) {
        printf("\nCTRL+Z pressed.\nI'm still the son's process.\nMy id is: %i\n", getpid());
        printf("Caught signal %d.\nAfter I finish (son process), my dad will finish too.", sig_num);
        fflush(stdout);
        kill(getppid(), SIGTERM);
        exit(EXIT_SUCCESS);
    }
}