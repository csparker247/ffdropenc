#!/bin/sh

# optname YouTube 1080p HD (Standard)

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_YouTube1080-debug.log
			OUTFILE="$RAWNAME"_YouTube1080.mp4
			
		# Video pass
			echo "Encoding YouTube 1080p HD (Standard) Version of $INFILE"
			# Type of encode: 0 = single pass, 1 = first pass, 2 = second pass
			PASS="0"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 10M -maxrate 10M -bufsize 3M -pix_fmt yuv420p -vf scale=1920:-1 -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" \
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
