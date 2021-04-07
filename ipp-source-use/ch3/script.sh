#!/bin/bash

OUTPUT_FILE=$1

for n in {2..10..2}; do
  echo "n = $n" >> $OUTPUT_FILE
  
  for run in {1..5}; do
    echo "($run) Calculando $((n * (10**6))))"; 
    val=$(mpiexec -n 1 ./mpi_trap3 $((n * (10**6))))
    echo -e "\t($run) $val s" >> $OUTPUT_FILE; 
  done 
done