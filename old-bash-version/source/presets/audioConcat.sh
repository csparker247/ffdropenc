#!/bin/sh

# Next line used by ffdropenc.sh
# optname Concatenate Audio (Stream Level)

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="Concatenated Audio"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="1"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)
THIS_FRAMES="0"

# Encoding options
VSUFFIX="Concat"
VEXTENSION="wav"
TPL_SUFFIX="concat.list"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"

		if [[ $index == 0 ]]; then
			setOutputs "$INPUT_FILE"
		fi
		
		echo "file '$INPUT_FILE'" >> "$TWOPASSLOG"	
done

# Encode audio
	echo "Encoding $CONSOLENAME Version of input files..."
	ffmpeg $(echo $SEQ_OPTS) -f concat -i "$TWOPASSLOG" \
	-y "$OUTFILE" \
	2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
	
# Track encoding progress	
	getProgress FFMPEG

# Cleanup
	cleanLogs	

exit 0