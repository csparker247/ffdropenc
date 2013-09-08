#!/bin/sh

# optname Web 1080p HD (High Grain)

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_Web1080-debug.log
			OUTFILE="$RAWNAME"_Web1080.mp4
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding Web 1080p HD (High Grain) Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 10M -pix_fmt yuv420p -preset slow -tune grain -profile:v baseline -vf scale=1920:-1 -c:a libfdk_aac -b:a 320k -ar 44.1k -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Move the faststart atom
		echo Moving moov atom.	
		"$qtfaststart" "$OUTFILE"
				
		# Update progress
			count=$(echo "scale=3; ($count+1)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		# Cleanup
			rm "$ERRLOG"			
	done
exit 0
