#!/bin/sh


cnt = 20

for i in $(seq 37  47)
do
    xterm -hold -e p '192.168.0.154' $i  &
done

#for number in 1 2 3 4 5 
#do
#    xterm -hold -e p '192.168.0.154' 10  &
#done
#echo "done.."
