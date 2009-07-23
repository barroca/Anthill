#!/bin/bash -x

PROJECT=/home/speed/george/void_3.1/eventAPI/samples/knn
LOG=$PROJECT/log
CONFIG=$PROJECT/config

numMachines=3
command="./initScript -test test_1500.dat -train train_base_1.5M_50d_15c.dat -k 50 -d 50"

# Para cada host possivel do le_base
for actualNumMachines in `seq 1 $numMachines`; 
	# Para cada host possivel do atributo
	#do for atributo in `seq 1 $atributoNodes`;
do	cd $PROJECT;
	cp $CONFIG/conf-especial$actualNumMachines.xml ./conf.xml;
	cp config/constants1.h ./constants.h
	make clean;make;
	$command;
	mkdir $LOG/machines/$actualNumMachines;
	mv $LOG/*.txt $LOG/machines/$actualNumMachines;
done
echo "tests done!"


