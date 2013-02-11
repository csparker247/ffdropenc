#!/bin/sh

# optname AppleTV

# Encode each file for AppleTV
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			VERRLOG="$RAWNAME"_appletv-debug.log
			OUTFILE="$RAWNAME"_APPLETV.mp4

		# Vimeo pass 
			echo Encoding AppleTV Version of "$INFILE"
			ffmpeg -i "${filelist[$i]}" -c:a libfdk_aac -b:a 320k -c:v libx264 -pix_fmt yuv420p -preset slow -profile:v main -level 40 -maxrate 15MB -bufsize 10MB -threads 0 "$OUTFILE" 2>"$VERRLOG"
				# Progress update
				count=$(echo "scale=3; $count+1.0" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
						
		# Cleanup
			rm "$VERRLOG"			
	done
exit 0