#!/bin/sh

# optname [SINGLE-PASS TEMPLATE]

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_[TEMPLATE]-debug.log
			OUTFILE="$RAWNAME"_[TEMPLATE].mp4
			
		# Video pass
			echo "Encoding [SINGLE-PASS TEMPLATE] Version of $INFILE"
			# Type of encode: 0 = single pass, 1 = first pass, 2 = second pass
			PASS="0"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 2500K -pix_fmt yuv420p -profile:v baseline -vf scale=480:-1 -c:a libfdk_aac -b:a 192k -ar 44.1k -y "$OUTFILE" \
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
