#!/bin/sh

# optname YouTube 1080p HD (Standard)

# Encode each file for YouTube
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			YERRLOG="$RAWNAME"_YT1080-debug.log
			YOUFILE="$RAWNAME"_YT1080.mp4

				
		# YouTube Pass
			echo "Encoding YouTube 1080p (Standard) Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 10M -maxrate 10M -bufsize 3M -pix_fmt yuv420p -vf scale=1920:-1 -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k "$YOUFILE" 2>"$YERRLOG"
				# Progress update
				count=$(echo "scale=3; $count+0.8" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo Moving moov atom.	
			"$qtfaststart" "$YOUFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.2" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
		# Cleanup
			rm "$YERRLOG"

		done