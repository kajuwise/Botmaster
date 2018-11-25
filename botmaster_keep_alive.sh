#!/bin/bash

#=============================================CONF==============================================================================

#PLACE HERE YOUR FIRST TIME BOTMASTER EXECUTION COMMAND HERE
startCommand='./botmaster2013 -r neve'
#PLACE HERE YOUR BOTMASTER RESTART COMMAND HERE
restartCommand='./botmaster2013 -g -r neve'


#============================================CYCLE==============================================================================
startCounter=0;
while :
do
	#STARTCOUNTER++
	startCounter=`expr $startCounter + 1`	
	
	#START/RESTART
	if [ "$startCounter" -gt 1 ]; then
		echo "Botmaster stopped running. Was stopped by fault. Start number #$startCounter"
		$restartCommand
    	else
		echo "Starting Botmaster for the first time"		
		$startCommand
	fi
	
	#EXIT REASON
	if [ $? -eq 2 ]
		then
			echo "Botmaster stopped running. Was stopped by user."
			exit 0
	fi
done
