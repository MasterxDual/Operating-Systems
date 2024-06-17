/*
	SO Practico - signals
    Ejemplo sencillo de manejo de señales
*/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* Variables globales */
int pid;
/* Fin variables globales */

void sigmanager(int sig_num) {
	/* Reasignar la señal */
	signal( sig_num, sigmanager );
	
	printf("Capturamos la señal: %d\n", sig_num);

    /* si queremos escribir algo es importante forzar la salida
       si queremos ver inmediatamente */
	fflush(stdout);
}

int main() {

	pid = fork();
	signal(SIGINT, sigmanager);

	/* loop infinito */
	while ( 1 ) {
		printf(".\n");
		sleep(1);
	}
	
	return 0;
}
