#!/bin/sh

# optname AppleTV

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_appletv-debug.log
			OUTFILE="$RAWNAME"_AppleTV.mp4
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding AppleTV Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -i "${filelist[$i]}" -c:a libfdk_aac -b:a 320k -c:v libx264 -pix_fmt yuv420p -preset slow -profile:v main -level 40 -maxrate 15MB -bufsize 10MB -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh

		# Update progress
			count=$(echo "scale=3; ($count+1)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
			
		# Cleanup
			rm "$ERRLOG"			
	done
exit 0
