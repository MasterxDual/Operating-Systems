/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 1, Ejercicio 2
 Objetivo: Aprender a utilizar fork
 Fecha: 14/03/2024
\**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

/* Variable que devuelve un valor entero dependiendo del error que suceda en el programa. Aquí se la declara solamente, utilizando
la palabra extern para no usar memoria por si esa variable no es utilizada, es decir, por si no ocurre ningún error.
En el caso que suceda un error, la utilizaremos para imprimir el código del error.*/
extern int errno;
int main(int argc, char const *argv[])
{
	int pid;

	/*Utilizamos la funcion fork, es decir a partir de aquí clonamos el proceso 1 junto al proceso 2.
	   Esta funcion fork devolvera un valor dependiendo lo que suceda (y será almacenada en pid),
	   donde son indicadas sus opciones en las clausulas if. */
	pid = fork();
	if (pid == 0) {
		/*En este caso estamos parados en el proceso hijo del proceso que lo clonó (proceso padre).
		   Aquí imprimimos un mensaje indicando que es el proceso hijo.
		   En el proceso hijo el pid será igual a 0. */
		printf("Hi, I'm the son process.\n");
	} else if (pid < 0) {
		/* En este bloque tendremos un error si hay una falla u error en la funcion fork.
		   Entonces imprimimos el código del error, luego utilizamos la función perror(char* message) para imprimir por pantalla
		   la descripción del error que sucedió junto con un mensaje previo a la descripción. */
		printf("Value of error: %d\n", errno);
		perror("Program will finish:");
		exit(EXIT_FAILURE);
	} else {
		/*En este bloque, imprimimos un mensaje indicando que es el proceso padre. */
		printf("Hi, I'm the father process.\n");
	}

	/* Si no sale el programa repentinamente por algún error (con el exit()), entonces el código llegará aquí y saldrá del programa
	   exitosamente. */
	exit(EXIT_SUCCESS);
	return 0;
}
