#!/bin/sh

# progress.sh - For accurately tracking ffmpeg progress.

# Check whether this is a ffmpeg or x264 encode	
sleep 1
if [[ $ENCODER == "FFMPEG" ]]; then
	# Get ffmpeg Process ID
		PID=$( ps -ef | grep "ffmpeg" | grep -v "grep" | awk '{print $2}' )

	# Get video duration in frames
		duration=$(ffmpeg -i "${filelist[$index]}" 2>&1 | sed -n "s/.* Duration: \([^,]*\), start: .*/\1/p")
		fps=$(ffmpeg -i "${filelist[$index]}" 2>&1 | sed -n "s/.*, \(.*\) tbr.*/\1/p")
		hours=$(echo $duration | cut -d":" -f1)
		minutes=$(echo $duration | cut -d":" -f2)
		seconds=$(echo $duration | cut -d":" -f3)
		FRAMES=$(echo "($hours*3600+$minutes*60+$seconds)*$fps" | bc | cut -d"." -f1)
	
	# While ffmpeg runs, process the log file for the current frame, display percentage progress
		while ps -p $PID>/dev/null ; do
			currentframe=$(tail -n 1 "$ERRLOG" | sed 's/frame=\(.*\)fps=.*/\1/'| sed 's/ //g')
			if [[ -n "$currentframe" ]]; then
				THIS_PROG=$(echo "scale=3; ($currentframe/$FRAMES)" | bc)
				INTER_PROG=$(echo "scale=3; ($THIS_PROG/$NUM_PASSES)+$count" | bc)
				PROG=$(echo "scale=3; ($INTER_PROG/$args)*100.0" | bc)
				echo "PROGRESS:$PROG"
				sleep 1
			fi
		done
elif [[ $ENCODER == "X264" ]]; then
	# Get x264 Process ID
		PID=$( ps -ef | grep "x264" | grep -v "grep" | awk '{print $2}' )

	while ps -p $PID>/dev/null ; do
		CURRENT_OVER_FRAMES=$(tail -n 1 "$ERRLOG" | awk '/frames\,/ { print $2 }')
		if [[ -n "$CURRENT_OVER_FRAMES" ]]; then
			INTER_PROG=$(echo "scale=3; ($CURRENT_OVER_FRAMES/$NUM_PASSES)+$count" | bc)
			PROG=$(echo "scale=3; ($INTER_PROG/$args)*100.0" | bc)
			echo "PROGRESS:$PROG"
		fi
		sleep 1
	done
fi