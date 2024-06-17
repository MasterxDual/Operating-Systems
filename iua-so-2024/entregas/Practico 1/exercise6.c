/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 1, Ejercicio 6
 Objetivo: Capturando señales
 Fecha: 19/03/2024
\**************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void handle_sigint(int sig_num);
void handle_sigtstp(int sig_num);

extern int errno;
int signalAction = 0;

int main(int argc, char const *argv[])
{
    /*Con el SIGINT capturo la señal CTRL+C y con el SIGTSTP capturo la señal
    CTRL+Z. Verifico si ocurre algún error mientras a su vez invoco a las dos funciones signal
    Hago un bucle infinito para poder repetir señales hasta que pulse la señal CTRL+Z. */
    
    if(signal(SIGINT, handle_sigint) == SIG_ERR || signal(SIGTSTP, handle_sigtstp) == SIG_ERR) {
        printf("Error number:%d\n", errno);
        perror("Error setting signal handler");
    }

    /*Dentro del bucle verifico si la variable global es 1 (esto ocurre cuando una
    señal es capturada satisfactoriamente) e imprimo por pantalla un mensaje, cambiando el valor de la 
    variable global a 0 para poder volver a recibir otra señal.*/
    while(1) {
        if(signalAction == 1) {
            printf("Signal succesfully received.\n");
            signalAction = 0;
        }
    }
    return 0;
}

/**Captura la señal CTRL+C imprimiendo en pantalla el código de la señal capturada, luego limpia el búfer para poder imprimir 
 *inmediatamente lo que aparezca por consola.
 * sig_num: código de la señal capturada.
*/
void handle_sigint(int sig_num) {
    if(sig_num == SIGINT) {
        printf("\nCTRL+C pressed. Caught signal %d.\n", sig_num);
        signalAction = 1;
    }

    fflush(stdout);
}

/**Capturo la señal CTRL+Z imprimiendo en pantalla el código de la señal capturada, luego se aborta el programa con un exit.
 * sig_num: código de la señal capturada. 
*/
void handle_sigtstp(int sig_num) {
    if(sig_num == SIGTSTP) {
        printf("\nCTRL+Z pressed. Caught signal %d.\nProgram will be aborted.\n", sig_num);
        signalAction = 1;
    }

    fflush(stdout);
    exit(0);
}