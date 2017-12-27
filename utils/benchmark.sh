#!/bin/bash

if [[ $1 == '' ]] ; then
    echo "Usage: ./benchmark.sh [program] <iterations>"
    echo "By default, iterations is 1. If provided, the executable will be"
    echo "executed for that many iterations and statistics reported for each"
    echo "iteration."
fi

if [[ $2 =~ ^[0-9]+$ ]] ; then
    iterations=$2
else
    iterations=1
fi

printf "Elapsed Time\tSystem Time\tUser Time\tCPU Usage\tMax Mem (kb)\tAvg Mem (kb)\n"
for i in $(seq 1 $iterations) ; do
    /usr/bin/time -f "%E\t\t%S\t\t%U\t\t%P\t\t%M\t\t%K" $1
done
