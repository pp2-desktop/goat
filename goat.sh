#!/bin/sh


for number in 1 2 3 4 5 
do
    xterm -hold -e p '192.168.0.154' 10  &
done
echo "done.."
