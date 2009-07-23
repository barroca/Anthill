#!/bin/bash -x

PROJECT=/home/speed/george/void_3.1/eventAPI/samples/knn
LOG=$PROJECT/log
CONFIG=$PROJECT/config

numMachines=10
command="./initScript -test test_1500.dat -train train_base_1.5M_50d_15c.dat -k 50 -d 50"

# Para cada host possivel do le_base
for actualNumMachines in `seq 1 $numMachines`; 
	# Para cada host possivel do atributo
	#do for atributo in `seq 1 $atributoNodes`;
do	cd $PROJECT;
	cp $CONFIG/conf$actualNumMachines.xml ./conf.xml;

	$command;
	mkdir $LOG/$actualNumMachines;
	mv $LOG/*.txt $LOG/$actualNumMachines;
	for machKill in `seq 1 12`; 
	do
		ls
#		ssh crystal$machKill killall -9 main;	
	done

done
echo "tests done!"


