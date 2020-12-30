#!/bin/sh

# Next line used by ffdropenc.sh
# optname MP3 (Audio Only)

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="${preset_name[$enc_type]}"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="1"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)

# Encoding options
VSUFFIX="AudioOnly"
VEXTENSION="mp3"

TARGET_ARATE="0"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"

		setOutputs "$INPUT_FILE"
		THIS_FRAMES="$(getLength "$INPUT_FILE")"

		# Video pass
			echo "Encoding $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" \
			-c:a libmp3lame -q:a "$TARGET_ARATE" \
			-y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &

		# Track encoding progress
			getProgress FFMPEG

		# Update progress
			updateProgress

		# Cleanup
			cleanLogs
	done
exit 0
