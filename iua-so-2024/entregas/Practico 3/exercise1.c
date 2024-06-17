/**************************************************************\
 Autor: Agustin Brambilla
 Practico: 3, Ejercicio 1
 Objetivo: Imprimir comandos escritos en consola
 Fecha: 11/04/2024
\**************************************************************/

#include <stdio.h>

int main(int argc, char *argv[]) {

    /*It dynamically cycles through the arrays that are within the main array. Each arrangement
    contains every word typed in the Linux command shell, consisting of a sentence
    written and displayed in the same console.*/
    printf("Memory direction Command\n");
    for (int z = 0; z < argc; z++) {
        printf("%p %s\n", &argv[z], argv[z]);
    }

    return 0;
}
