#!/bin/sh

# Next line used by ffdropenc.sh
# optname Generic SD

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="${preset_name[$enc_type]}"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="1"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)

# Encoding options
VSUFFIX="GenericSD"
VEXTENSION="mp4"
VENCODER="libx264"
PIX_FMT="yuv420p"
CRF="20"
TARGET_VRATE="3M"
TARGET_BUFFER="6M"
TARGET_WIDTH="854"
TARGET_HEIGHT="480"
TARGET_DAR="16/9"

TARGET_ARATE="192k"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"
		
		setOutputs "$INPUT_FILE"
		THIS_FRAMES="$(getLength "$1")"

		# Video pass
			echo "Encoding $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" \
			-c:v "$VENCODER" -crf "$CRF" -maxrate "$TARGET_VRATE" -bufsize "$TARGET_BUFFER" -pix_fmt "$PIX_FMT" -profile:v high -level 42 \
				-vf \
					"scale=iw*sar:ih,
					scale=
						'w=if(lt(dar, $TARGET_DAR), trunc(oh*a/2)*2, min($TARGET_WIDTH,ceil(iw/2)*2)): 
     					h=if(gte(dar, $TARGET_DAR), trunc(ow/a/2)*2, min($TARGET_HEIGHT,ceil(ih/2)*2))', 
					setsar=1" \
				-movflags faststart \
			-c:a libfdk_aac -b:a "$TARGET_ARATE" \
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