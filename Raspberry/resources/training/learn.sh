#!/bin/bash

now=$(date +"%Y_%m_%d_%H-%M")

echo -n "[" >> totalReward.txt
cd ../../build
make

for i in {1..20}
do
    #spd-say "3"
    #sleep 2
    spd-say "2"
    sleep 2
    spd-say "1"
    sleep 2
    spd-say "start episode $i"
    sleep 1

    echo  >> ../resources/training/trainingRuns/trainingRun_$now.txt
    echo "#########################################################" >> ../resources/training/trainingRuns/trainingRun_$now.txt
	echo "Episode $i" >> ../resources/training/trainingRuns/trainingRun_$now.txt
	sudo ./Launcher -r | tee -a ../resources/training/trainingRuns/trainingRun_$now.txt
	#echo -n "10," >> ../resources/training/totalReward.txt
	#echo "###########################" >> test/qValues_$now.txt
	#cat qValues.txt >> test/qValues_$now.txt #_$(date +"%Y_%m_%d_%H-%M-%S").txt
    #echo " " >> test/qValues_$now.txt
    #sleep 1
    spd-say "fire"
    sleep 2
done

sed -i '$ s/.$//' ../resources/training/totalReward.txt
echo "]" >> ../resources/training/totalReward.txt
cat ../resources/training/qValues.txt >> ../resources/training/qValues/qValues_saved_$now.txt
cat ../resources/training/qValues_blank.txt > ../resources/training/qValues.txt
spd-say "all done"
