#!/bin/sh

# Global Functions

analyze () {
	ffprobe -i "$1" -loglevel quiet -of flat -select_streams v \
	-show_entries stream=codec_name,width,height,pix_fmt,level,bit_rate,channels > "$ERRLOG"
	THIS_VEXTENSION=""
	THIS_VCODEC="$(sed -n /codec_name/p "$ERRLOG" | sed 's/.*codec_name="\(.*\)"/\1/')"
	THIS_FRAMES="$(getLength "$1")"
	THIS_VRATE="$(sed -n /bit_rate/p "$ERRLOG" | sed 's/.*bit_rate="\(.*\)"/\1/')"
	THIS_WIDTH="$(sed -n /width/p "$ERRLOG" | sed 's/.*width=\(.*\)/\1/')"
	THIS_HEIGHT="$(sed -n /height/p "$ERRLOG" | sed 's/.*height=\(.*\)/\1/')"
}

getLength () {
	# Get video duration in frames
	echo $(ffprobe -i "$1" -loglevel quiet -of flat -select_streams v:0 -show_entries stream=nb_frames | sed 's/.*frames="\(.*\)"/\1/')
}

compareParams () {
	if [ "$1" -ge "$2" ]; then
		return 0
	else
		return 1
	fi
}

setOutputs () {
	# Remove the extension and make filenames for logs and output.
	INPUT_NAME="$(basename "$1")"
	# Set output path
	if [[ "$useNewOutput" == "1" ]]; then
		OUTPATH="$newOutputPath"
	else
		OUTPATH="$(dirname "$1")"
	fi

	# Remove numbers & extension if sequence, only extension otherwise
	if [[ "$INPUT_NAME" =~ .*\.($sequence_exts) ]]; then
		SEQ_OPTS="-f image2 -r $enc_fps"
		X_OPTS="--fps $enc_fps"
		RAWNAME="$(echo "$INPUT_NAME" | sed 's/%[0-9]*d\..*//')"
	else
		RAWNAME="$(echo "$INPUT_NAME" | sed 's/\(.*\)\..*/\1/')"
	fi

	# Add underscore if there's not a separator in RAWNAME
	[[ "$RAWNAME" != *_ ]] && [[ "$RAWNAME" != *- ]] && [[ "$RAWNAME" != *" " ]] && RAWNAME="$RAWNAME"_

	ERRLOG="${OUTPATH}/${RAWNAME}${VSUFFIX}.log"
	TWOPASSLOG="${OUTPATH}/${RAWNAME}${TPL_SUFFIX}"
	OUTFILE="${OUTPATH}/${RAWNAME}${VSUFFIX}.${VEXTENSION}"
	AUDIOFILE="${OUTPATH}/${RAWNAME}${ASUFFIX}.${AEXTENSION}"
}

getProgress () {
	# Check whether this is a ffmpeg or x264 encode	
	sleep 1
	if [[ $ENCODER == "FFMPEG" ]]; then
		# Get ffmpeg Process ID
			PID=$( ps -ef | grep "ffmpeg" | grep -v "grep" | awk '{print $2}' )
	elif [[ $ENCODER == "X264" ]]; then
		# Get x264 Process ID
			PID=$( ps -ef | grep "x264" | grep -v "grep" | awk '{print $2}' )
	fi

	# While encoder runs, process the log file for the current frame, display percentage progress
	while ps -p $PID>/dev/null ; do
		if [[ $ENCODER == "FFMPEG" ]]; then
			CURRENTFRAME=$(tail -n 1 "$ERRLOG" | sed 's/frame=\(.*\)fps=.*/\1/'| sed 's/ //g')
		elif [[ $ENCODER == "X264" ]]; then
			CURRENTFRAME=$(tail -n 1 "$ERRLOG" | awk '/frames\,/ { print $2 }' | sed 's:/[0-9]*::')
		fi
		if [[ -n "$CURRENTFRAME" ]]; then
			tempFINISHED=$(echo "$FINISHEDFRAMES + $CURRENTFRAME" | bc)
			PROG=$(echo "scale=3; ($tempFINISHED/$TOTALFRAMES)*100.0" | bc)
			echo "PROGRESS:$PROG"
		fi
		sleep 1
	done
}

updateProgress () {
	FINISHEDFRAMES=$(echo "$FINISHEDFRAMES + $THIS_FRAMES" | bc)
	PROG=$(echo "scale=3; ($FINISHEDFRAMES/$TOTALFRAMES)*100.0" | bc)
	echo PROGRESS:"$PROG"
}

cleanLogs () {
	[[ -e "$ERRLOG" ]] && rm "$ERRLOG"
	[[ -e "${TWOPASSLOG}*.log" ]] && rm "${TWOPASSLOG}*.log"
	[[ -e "${TWOPASSLOG}.mbtree" ]] && rm "${TWOPASSLOG}.mbtree"
}