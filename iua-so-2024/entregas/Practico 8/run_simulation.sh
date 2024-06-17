#!/bin/bash

# Generar archivos de entrada
./generate_files.sh

# Compilar el programa
gcc -o sim sim.c

# Ejecutar la simulación para todos los archivos y algoritmos
> results.csv
for alg in FIFO LRU OPT; do
  for file in SET*.dat; do
    ./sim -a $alg $file
  done
done

