#!/bin/bash

echo "a,k,system,user,real" > "tiempos.csv"

for ((k = 10; k <= 100; k = k + 10)); do
    for ((a = 10; a <= 100; a = a + 10)); do
        echo -n "$a,$k," >> "tiempos.csv"
        /usr/bin/time -f "%S,%U,%e" -a -o "tiempos.csv" ./main.bin -i "data/train.csv" -q "data/test.csv" -m 1 -a $a -k $k
    done
done