#!/bin/bash

filename=results/$(date -d "today" +"%Y%m%d%H%M%S").csv
touch $filename

echo "n,time_real,time_user,time_sys" >> $filename

for i in {1..5}
do
  OUTPUT=$( { /usr/bin/time -f "%e,%U,%S" ./C/main 1000 1000 1000 1; } 2>&1 )

  #start=$(date +%s.%N)
  #./C/main 100 100 100 $i
  #end=$(date +%s.%N)
  #runtime=$(echo "$end - $start" | bc)


  echo "$i,${OUTPUT}" >> $filename


done

