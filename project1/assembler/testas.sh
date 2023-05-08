#!/bin/sh

make
for var in 1 2 3 4 5 6 7 8
do
	echo $var
  ./assembler ./test$var.as ./test$var.mc
done
