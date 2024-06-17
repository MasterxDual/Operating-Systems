/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 1, Ejercicio 5
 Objetivo: Aprender a utilizar señales
 Fecha: 15/03/2024
\**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

extern int errno;
int main(int argc, char const *argv[])
{
	/*pid_son es el id de proceso que devolverá la función wait, y justamente ese id es el del hijo que será terminado 
	   satisfactoriamente, donde luego seguirá ejecutándose el del padre ya que la función wait hace eso.
	   pid es la misma variable de los anterior programas
	   status es un entero que será devuelto por la función wait a través de su dirección de memoria, por eso se inserta el 
	   ampersand como parámetro. Status indica el valor de salida del proceso hijo. */
	pid_t pid_son;
	pid_t pid;
	int status;
    int seconds;

	pid = fork();

	if (pid < 0) {
		printf("Value of error: %d\n", errno);
		perror("Program will finish:");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		//Se ejecutará este bloque antes de que el proceso padre se ejecute.
		printf
			("Hi, I'm the process son of id %i. I will sleep now, so please wake me up fast so I can run again.\n",
			 getpid());
        /*Mandamos a estado 'bloqueado' al proceso hijo. Volverá al estado 'en ejecución' una vez finalice el tiempo, 
        o una vez sea interrumpido por una señal (que eso haremos).*/
        seconds = sleep(10000);
	} else {
		/*En este bloque, llegamos al proceso padre, por ende hacemos uso de la función wait, que retorna el id del proceso
		   hijo. */
		pid_son = wait(&status);

		/*Puede retornar -1 si ocurrió un error, por ende atrapamos esa excepción */
		if (pid_son < 0) {
			printf("Value of error: %d\n", errno);
			perror("Program will finish:");
			exit(EXIT_FAILURE);
		}

		/*Aquí analizamos el valor del status de salida del proceso hijo. A continuación detallamos que hace cada macro:
		   WIFEXITED: retorna true si el proceso hijo terminó satisfactoriamente.
		   WEXITSTATUS: retorna el estado de salida del hijo.
		   WIFSIGNALED: retorna true si el proceso hijo fue terminado por una señal.
		   WTERMSIG: retorna el número de la señal que provocó que el proceso hijo terminara.
		   WIFSTOPPED: retorna true si el proceso hijo fue detenido por la entrega de una señal.
		   WSTOPSIG: retorna el número de la señal que provocó que el niño se detuviera. 
		   WIFCONTINUED: retorna true si el proceso hijo fue reasumido por la entrega de SIGCONT */
		if (WIFEXITED(status)) {
			printf("Exited, status: %d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			psignal(WTERMSIG(status), "Killed by signal:");
		} else if (WIFSTOPPED(status)) {
			psignal(WSTOPSIG(status), "Stopped by signal:");
		} else if (WIFCONTINUED(status)) {
			printf("Continued process\n");
		}
		/*Imprimimos por pantalla el id del proceso padre, el del hijo y el valor del estado de salida del hijo. */
		printf("Hi, I'm the parent process, my process id is %i.\n",
			   getpid());
		printf("The proccess id of my son is %i.\n", pid_son);
		printf("The exit status of my son's process is %d.", status);
	}

	//Salimos satisfactoriamente del programa en el caso que no haya ocurrido ningún error.
	exit(EXIT_SUCCESS);
	return 0;
}
