#!/bin/sh

# Next line used by ffdropenc.sh
# optname Bluray 1080p (H.264, AC-3, 30p)

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="Bluray"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="3"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)

# Encoding options
VSUFFIX="BD_VIDEO"
TPL_SUFFIX="2pass"
VEXTENSION="264"
TARGET_VRATE="39000"
TARGET_MAXRATE="40000"
TARGET_BUFFER="30000"
TARGET_WIDTH="1920"
TARGET_HEIGHT="1080"

ASUFFIX="BD_AUDIO"
AEXTENSION="ac3"
TARGET_ARATE="640k"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"
		
		setOutputs "$INPUT_FILE"
		THIS_FRAMES="$(getLength "$INPUT_FILE")"

		# Video pass
			echo "Encoding 1st Pass, $CONSOLENAME Version of $INPUT_NAME"
			x264 --quiet \
			--bitrate "$TARGET_VRATE" --bluray-compat --pic-struct --vbv-maxrate "$TARGET_MAXRATE" --vbv-bufsize "$TARGET_BUFFER" \
			--preset veryslow --tune film $(echo $X_OPTS) --level 4.1 \
			--keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 \
			--sar 1:1 --video-filter resize:width="$TARGET_WIDTH",height="$TARGET_HEIGHT",fittobox=both \
			--fps 30000/1001 --force-cfr \
			--pass 1 --stats "${TWOPASSLOG}.log" -o "$OUTFILE" "$INPUT_FILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress X264

		# Update progress
			updateProgress

		# Video pass
			echo "Encoding 2nd Pass, $CONSOLENAME Version of $INPUT_NAME"
			x264 --quiet \
			--bitrate "$TARGET_VRATE" --bluray-compat --pic-struct --vbv-maxrate "$TARGET_MAXRATE" --vbv-bufsize "$TARGET_BUFFER" \
			--preset veryslow --tune film $(echo $X_OPTS) --level 4.1 \
			--keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 \
			--sar 1:1 --video-filter resize:width="$TARGET_WIDTH",height="$TARGET_HEIGHT",fittobox=both \
			--fps 30000/1001 --force-cfr \
			--pass 2 --stats "${TWOPASSLOG}.log" -o "$OUTFILE" "$INPUT_FILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress X264

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