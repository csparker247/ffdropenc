#!/bin/sh

# Global Functions

# analyze $filepath $outputpath
# Collect media information into temp variables.
# NOTE: Presets should account for the log file created by this process.
analyze () {
	ffprobe -i "$1" -loglevel quiet -of flat -select_streams v \
	-show_entries stream=codec_name,width,height,pix_fmt,level,bit_rate,nb_frames,channels > "$2"
	THIS_VEXTENSION=""
	THIS_VCODEC="$(sed -n /codec_name/p "$ERRLOG" | sed 's/.*codec_name="\(.*\)"/\1/')"
	THIS_FRAMES="$(sed -n /nb_frames/p "$ERRLOG" | sed 's/.*frames="\(.*\)"/\1/')"
	THIS_VRATE="$(sed -n /bit_rate/p "$ERRLOG" | sed 's/.*bit_rate="\(.*\)"/\1/')"
	THIS_WIDTH="$(sed -n /width/p "$ERRLOG" | sed 's/.*width=\(.*\)/\1/')"
	THIS_HEIGHT="$(sed -n /height/p "$ERRLOG" | sed 's/.*height=\(.*\)/\1/')"
}

# getLength $filepath
# Get video duration in frames
getLength () {
		tempLENGTH=$(ffprobe -i "$1" -loglevel quiet -of flat -select_streams v:0 -show_entries stream=nb_frames | sed 's/.*frames="\(.*\)"/\1/' | tail -1)
		if [[ "$tempLENGTH" != [[:digit:]]* ]]; then
			duration=$(ffprobe -i "$1" -loglevel quiet -of flat -select_streams v:0 -show_entries stream=duration | sed 's/streams.*duration="\(.*\)"/\1/' | tail -1)
			fps=$(ffprobe -i "$1" -loglevel quiet -of flat -select_streams v:0 -show_entries stream=r_frame_rate | sed 's/streams.*r_frame_rate="\(.*\)"/\1/' | tail -1)
			tempLENGTH=$(echo "scale=0; $duration*($fps)/1" | bc)
		fi
		echo $tempLENGTH
}

# getFPS $filepath
# Get video FPS
getFPS () {
		tempFPS=$(ffprobe -i "$1" -loglevel quiet -of flat -select_streams v:0 -show_entries stream=r_frame_rate | sed 's/.*r_frame_rate="\(.*\)"/\1/' | tail -1)
		echo $tempFPS
}

# compareParams $param $baseline_param
# Used to test a parameter against a baseline parameter.
# Parameters are expected to be numeric (e.g. bitrates in kilobytes)
compareParams () {
	if [ "$1" -ge "$2" ]; then
		return 0
	else
		return 1
	fi
}

# setOutputs $filepath 
# Sets up the common names and outputs used by all presets.
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

# getProgress $encoder
# While the encoder process runs, getProgress() reads the encoder's error log
# and prints progress as the percent of frames completed. Progress is only kept while
# function runs. Follow with updateProgress() before switching to the next file to
# "save" progress. The PROGRESS:%d+ format is required for the Platypus progress bar.
getProgress () {
	# Check whether this is a ffmpeg or x264 encode	
	if [[ "$1" == "FFMPEG" ]]; then
		# Get ffmpeg Process ID
			PID=$( ps -ef | grep "ffmpeg" | grep -v "grep" | awk '{print $2}' )
	elif [[ "$1" == "X264" ]]; then
		# Get x264 Process ID
			PID=$( ps -ef | grep "x264" | grep -v "grep" | awk '{print $2}' )
	fi

	# While encoder runs, process the log file for the current frame, display percentage progress
	while ps -p $PID>/dev/null ; do
		if [[ "$1" == "FFMPEG" ]]; then
			CURRENTFRAME=$(tail -n 1 "$ERRLOG" | sed 's/frame=\(.*\)fps=.*/\1/'| sed 's/ //g')
		elif [[ "$1" == "X264" ]]; then
			CURRENTFRAME=$(tail -n 1 "$ERRLOG" | awk '/frames\,/ { print $2 }' | sed 's:/[0-9]*::')
		fi
		if [[ "$CURRENTFRAME" = [[:digit:]]* ]]; then
			tempFINISHED=$(echo "$FINISHEDFRAMES + $CURRENTFRAME" | bc)
			PROG=$(echo "scale=3; ($tempFINISHED/$TOTALFRAMES)*100.0" | bc)
			echo "PROGRESS:$PROG"
		fi
	done
}

# updateProgess
# Adds current file's frames to total number of frames completed. getProgress() only temporarily
# calculates this amount of work done. This function "saves" that work before switching
# to the next file in the list. The PROGRESS:%d+ format is required for the Platypus progress bar.
updateProgress () {
	FINISHEDFRAMES=$(echo "$FINISHEDFRAMES + $THIS_FRAMES" | bc)
	PROG=$(echo "scale=3; ($FINISHEDFRAMES/$TOTALFRAMES)*100.0" | bc)
	echo PROGRESS:"$PROG"
}

# cleanLogs
# Removes common log files if they exist.
cleanLogs () {
	[[ -e "$ERRLOG" ]] && rm "$ERRLOG"
	[[ -e "${TWOPASSLOG}.log" ]] && rm "${TWOPASSLOG}.log"
	[[ -e "${TWOPASSLOG}-0.log" ]] && rm "${TWOPASSLOG}-0.log"
	[[ -e "${TWOPASSLOG}.log.mbtree" ]] && rm "${TWOPASSLOG}.log.mbtree"
	[[ -e "${TWOPASSLOG}" ]] && rm "${TWOPASSLOG}"
}