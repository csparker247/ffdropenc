#!/bin/sh

# optname Vimeo 720p HD (2-Pass)

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="${filelist[$i]}"Vimeo720
			ERRLOG="$RAWNAME"_Vimeo720-2pass-debug.log
			OUTFILE="$RAWNAME"_Vimeo720-2pass.mp4

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="2"
						
		# Video Pass #1
			echo "Encoding 1st Pass, Vimeo 720p HD Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -i "${filelist[$i]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -pix_fmt yuv420p -vf scale=1280:-1 -an -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		
		# Video Pass #2	
			echo "Encoding 2nd Pass, Vimeo 720p HD Version of Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -i "${filelist[$i]}" -pass 2 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -maxrate 5M -bufsize 2M -pix_fmt yuv420p -vf scale=1280:-1 -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
			
		echo Moving moov atom.	
		"$qtfaststart" "$OUTFILE"
	
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
								
		# Cleanup
			rm "$ERRLOG"
			rm "$TWOPASS"-0.log
			rm "$TWOPASS"-0.log.mbtree	
	done

