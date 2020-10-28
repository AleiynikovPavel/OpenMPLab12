#!/bin/bash

rm B_*
rm A_*
rm C.txt
./lab12DP $1
mpirun -np $1 --oversubscribe  lab12MC 
