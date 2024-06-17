/*
    Uso de fork(), ver man 2 fork
*/
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

int main() {

    int pid;
    int mi_pid;

    pid = fork();
    /*  ejecutamos fork(), esta función 'crea' un nuevo proceso
        que es una copia del proceso invocante.
        La diferencia es que en el proceso que invoca se retorna
        el ID de proceso del nuevo proceso creado, y en el proceso
        creado retorna 0
    */

    if ( pid != 0 ) {
        /* estoy en padre */
        printf("Soy el padre");
        mi_pid = getpid();
        printf(" mi PID es %d\n", mi_pid);
    } else {
        /* estoy en hijo */
        printf("Soy el padre");
        mi_pid = getpid();
        printf(" mi PID es %d\n", mi_pid);
    }

    return 0;
}
