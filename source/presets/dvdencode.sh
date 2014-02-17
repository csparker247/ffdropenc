#!/bin/sh

# Next line used by ffdropenc.sh
# optname DVD NTSC (MPEG-2, AC-3)

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="DVD"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="3"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)

# Encoding options
VSUFFIX="DVD_VIDEO"
TPL_SUFFIX="2pass"
VEXTENSION="m2v"

ASUFFIX="DVD_AUDIO"
AEXTENSION="ac3"
TARGET_ARATE="448k"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"
		
		setOutputs "$INPUT_FILE"
		THIS_FRAMES="$(getLength "$1")"

		# Video pass
			echo "Encoding 1st Pass, $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" -pass 1 -passlogfile "$TWOPASSLOG" \
			-target ntsc-dvd \
			-an \
			-y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress FFMPEG

		# Update progress
			updateProgress

		# Video pass
			echo "Encoding 2nd Pass, $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" -pass 2 -passlogfile "$TWOPASSLOG" \
			-target ntsc-dvd \
			-an \
			-y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress FFMPEG

		# Update progress
			updateProgress

		# Audio Pass
			echo "Encoding AC-3 Audio, $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" \
			-c:a ac3 -b:a "$TARGET_ARATE" -ar 48000 \
			-y "$AUDIOFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress FFMPEG

		# Check for audio file
			if [[ ! -f "$AUDIOFILE" ]]; then
					echo "--------"
					echo "Warning: Audio file not created for ${INFILE}. Does it have audio?"
					echo "--------"
			fi

		# Update progress
			updateProgress

		# Cleanup
			cleanLogs			
	done
exit 0