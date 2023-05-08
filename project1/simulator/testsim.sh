#!/bin/sh
make
for var in 5 6 7 8 9
do
	echo $var
	./simulate.exe test$var.mc > test$var.out
done
