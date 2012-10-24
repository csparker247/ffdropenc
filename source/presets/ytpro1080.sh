#!/bin/sh
PATH=$PATH:./bin

# Encode each file for Vimeo
for (( i=0; i<${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="${filelist[$i]}"ytpro
			YERRLOG="$RAWNAME"_YT1080pro-debug.log
			YOUFILE="$RAWNAME"_YT1080pro.mp4

				
		# YouTube Pass
			echo "Encoding 1st Pass, YouTube 1080p HD (Professional) Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 45M -maxrate 55M -bufsize 3M -pix_fmt yuv420p -s 1920x1080 -an -y "$YOUFILE" 2>"$YERRLOG"1
				# Progress update
				count=$(echo "scale=3; $count+0.4" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo "Encoding 2nd Pass, YouTube 1080p HD (Professional) Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -pass 2 "$TWOPASS" -c:v libx264 -b:v 45M -maxrate 55M -bufsize 3M -pix_fmt yuv420p -s 1920x1080 -c:a libfaac -profile:a aac_low -b:a 320k -ar 48k -y "$YOUFILE" 2>"$YERRLOG"2
				# Progress update
				count=$(echo "scale=3; $count+0.4" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo Moving moov atom.	
			"$qtfaststart" "$YOUFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.2" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
		# Cleanup
			rm "$YERRLOG"1
			rm "$YERRLOG"2
			rm "$TWOPASS"-0.log
			rm "$TWOPASS"-0.log.mbtree

		done