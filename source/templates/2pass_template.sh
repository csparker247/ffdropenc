#!/bin/sh

# optname [TWO-PASS TEMPLATE]

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="${filelist[$i]}"[TEMPLATE]
			ERRLOG="$RAWNAME"_[TEMPLATE]-2pass-debug.log
			OUTFILE="$RAWNAME"_[TEMPLATE]-2pass.mp4

		# Video Pass #1
			echo "Encoding 1st Pass, [TWO-PASS TEMPLATE] Version of $INFILE"
			
			# Type of encode: 0 = single pass, 1 = first pass, 2 = second pass
			PASS="1"
			
			ffmpeg -i "${filelist[$i]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -pix_fmt yuv420p -vf scale=1280:-1 -an -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		
		# Video Pass #2	
			echo "Encoding 2nd Pass, [TWO-PASS TEMPLATE] Version of $INFILE"
		
			# Type of encode: 0 = single pass, 1 = first pass, 2 = second pass
			PASS="2"
		
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

