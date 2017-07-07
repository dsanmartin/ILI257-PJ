#!/bin/bash

folder=./results/$(date -d "today" +"%Y%m%d%H%M%S")
mkdir -p $folder;

#echo "n,time_real,time_user,time_sys" >> $filename

N=10
for i in {1..4}
do
	for j in {1..4}
	do
		#N*=10
		N=$((10*j))
		filename=$folder/${i}_${N}.csv
		touch $filename

		#echo "n,time_real,time_user,time_sys" >> $filename
		#OUTPUT=$( { /usr/bin/time -f "%e,%U,%S" ./C/main ${N} ${i}; } 2>&1 )	 	
	  	#echo "$N,${OUTPUT}" >> $filename

	  	echo "n,time" >> $filename
	  	start=$(date +%s.%N)
	  	./C/main $N $i
	  	end=$(date +%s.%N)
	  	runtime=$(echo "$end - $start" | bc)
	  	echo "$N,$runtime" >> $filename

	done
done

