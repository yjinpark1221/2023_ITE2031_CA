#!/bin/sh

for var in 5 6 7 8
do
	echo $var
	cp test$var.mc ../simulator/test$var.mc
done
