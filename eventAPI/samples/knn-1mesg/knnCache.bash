#!/bin/bash -x

PROJECT=/home/speed/george/void_3.1/eventAPI/samples/knn
LOG=$PROJECT/log
CONFIG=$PROJECT/config

numMachines=10
command="./initScript -test test_base_300_50d.dat -train /var/tmp/george/train_base_1.5M_50d_15c.dat -k 50 -d 50"

# Para cada host possivel do le_base
for actualNumMachines in `seq 2 $numMachines`; 
	# Para cada host possivel do atributo
	#do for atributo in `seq 1 $atributoNodes`;
do	cd $PROJECT;
	cp $CONFIG/conf-cache.xml ./conf.xml;
	cp $CONFIG/constants$actualNumMachines.h ./constants.h
	make clean;make;
	$command;
	mkdir $LOG/cache/
	mkdir $LOG/cache/$actualNumMachines;
	mv $LOG/*.txt $LOG/cache/$actualNumMachines;
done
echo "tests done!"


