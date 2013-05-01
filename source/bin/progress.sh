#!/bin/sh

#progress.sh - For accurately tracking ffmpeg progress. Single-pass encoding only 

# Get ffmpeg Process ID
	PID=$( ps -ef | grep "ffmpeg" | grep -v "grep" | awk '{print $2}' )
	
	
while ps -p $PID>/dev/null ; do
	currentframe=$(tail -n 1 "$ERRLOG" | awk '/frame=/ { print $2 }')
    if [[ -n "$currentframe" ]]; then
	    THIS_PROG=$(echo "scale=3; ($currentframe/$FRAMES)" | bc)
    	INTER_PROG=$(echo "scale=3; ($THIS_PROG+$count)" | bc)
    	PROG=$(echo "scale=3; ($INTER_PROG/$args)*100.0" | bc)
		echo PROGRESS:"$PROG"
	fi
	sleep 1
done

