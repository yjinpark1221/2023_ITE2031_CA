#!/bin/sh

for var in 1 2 3 4 5
do
	echo $var
	cp test$var.mc ../simulator/test$var.mc
done
