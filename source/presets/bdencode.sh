#!/bin/sh

# Next line used by ffdropenc.sh
# optname Bluray 1080p (H.264, AC-3)

# Next line used by this preset. Defaults to same as above.
CONSOLENAME="Bluray"

# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc. Used by progress tracker.
NUM_PASSES="1"
TOTALFRAMES=$(echo "$TOTALFRAMES * $NUM_PASSES" | bc)

# Encoding options
VSUFFIX="BD_VIDEO"
VEXTENSION="264"
VENCODER="libx264"
CRF="16"
PIX_FMT="yuv420p"
TARGET_VRATE="38M"
TARGET_MAXRATE="40M"
TARGET_BUFFER="30M"
TARGET_WIDTH="1920"
TARGET_HEIGHT="1080"
TARGET_DAR="16/9"

ASUFFIX="BD_AUDIO"
AEXTENSION="ac3"
TARGET_ARATE="320k"

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		INPUT_FILE="$(echo "${filelist[$index]}")"
		
		setOutputs "$INPUT_FILE"
		THIS_FRAMES="$(getLength "$INPUT_FILE")"
		THIS_TIMEBASE="$(getFPS "$INPUT_FILE")"
		THIS_KEYINT="$(echo "scale=3; $THIS_TIMEBASE" | bc | awk '{ printf("%.0f", $1) }')"

		# Encode video...
			echo "Encoding $CONSOLENAME Version of $INPUT_NAME"
			ffmpeg $(echo $SEQ_OPTS) -i "$INPUT_FILE" \
			-c:v "$VENCODER" -b:v "$TARGET_VRATE" -maxrate "$TARGET_MAXRATE" -bufsize "$TARGET_BUFFER" -pix_fmt "$PIX_FMT" \
			-preset veryslow -tune film -level 4.1 \
			-x264opts keyint="$THIS_KEYINT":bluray-compat=1:force-cfr=1:open-gop=1:slices=4:fake-interlaced=1:colorprim=bt709:transfer=bt709:colormatrix=bt709:sar=1/1 \
				-vf \
					"scale=iw*sar:ih,
					scale=
						'w=if(lt(dar, $TARGET_DAR), trunc(oh*a/2)*2, min($TARGET_WIDTH,ceil(iw/2)*2)): 
     					h=if(gte(dar, $TARGET_DAR), trunc(ow/a/2)*2, min($TARGET_HEIGHT,ceil(ih/2)*2))', 
					setsar=1" \
			-f H264 -r "$THIS_TIMEBASE" -y "$OUTFILE" \
			-c:a ac3 -b:a "$TARGET_ARATE" -ar 48000 \
			-y "$AUDIOFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			getProgress FFMPEG

		# Update progress
			updateProgress

		# Cleanup
			cleanLogs			
	done
exit 0