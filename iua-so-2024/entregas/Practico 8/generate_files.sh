#!/bin/bash

for SET in {01..10}; do 
  STR=$(for i in {1..30}; do DIV=$((7+1)); R=$(($RANDOM%$DIV)); echo -n $R; done); 
  for FRM in 3 4 6; do 
    file="SET$SET-$FRM.dat"; echo -e "$FRM\n$STR" > $file; 
  done; 
done

