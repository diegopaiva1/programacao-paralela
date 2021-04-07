#!/bin/bash
round()
{
  echo $(echo "scale=$2;(((10^$2)*$1)+0.5)/(10^$2)" | bc)
};

LIB=$1 
PROGRAM=$2
OUTPUT_FILE=$3

for ((p = 1; p <= 4; p *= 2)); do
  echo "--------------- p = $p ---------------" >> $OUTPUT_FILE
  echo "--------------- p = $p ---------------"

  for n in {100..900..200}; do
    echo "n = $n x 10^6" >> $OUTPUT_FILE
    VALUES=()

    for run in {1..5}; do
      echo "($run) Calculando n = $((n * 10**6))"

      if [[ "$LIB" == "omp" ]]; then
        OUTPUT=$( "$PROGRAM" "$p" $((n * 10**6)) 2)
      elif [[ "$LIB" == "mpi" ]]; then
        OUTPUT=$( mpiexec -n $p $PROGRAM $((n * (10**6))) 2) 
      else
        echo "Bad input $LIB"
        exit 1
      fi  
      
      echo -e "\t($run)" $OUTPUT >> $OUTPUT_FILE 
      TIME_S=$(echo "$OUTPUT" | grep -Eo '[0-9]+[.][0-9]+[s]') # capture time in seconds
      TIME_S=${TIME_S::-1} # remove 's' from string
      VALUES+=($TIME_S) # append
    done

    # Get sum of collected values to compute avg
    SUM=0
    for val in ${VALUES[@]}; do
      SUM=$(echo "$SUM + $val" | bc -l)
    done

    AVG=$(echo $(round $SUM/5 2))
    echo -e "\tavg =" $AVG >> $OUTPUT_FILE 

    # Compute standard deviation
    NUM=0
    for val in ${VALUES[@]}; do
      NUM=$(echo "$NUM + ($val - $AVG)^2" | bc -l)
    done

    SD=$(echo "sqrt($NUM/4)" | bc -l)
    SD=$(echo $(round $SD 2))
    echo -e "\tsd =" $SD >> $OUTPUT_FILE 
  done
done
