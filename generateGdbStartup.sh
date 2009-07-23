#!/bin/sh

# arguments: generateGdbFile.sh <file that calls initDs()> "<initScript args>"

#first breakpoint
echo -n "break $1:"
grep -n "initDs" $1 | sed "s/:.*$//"

#second breakpoint
echo -e "run $2\n step\n break callFDInit\n cont\n step\n $3 \n"
	



