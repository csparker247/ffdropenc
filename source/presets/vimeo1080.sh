#!/bin/sh

# optname Vimeo 1080p HD

# Encode each file for Vimeo
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			VERRLOG="$RAWNAME"_vim1080-debug.log
			VIMFILE="$RAWNAME"_VIM1080.mp4

		# Vimeo pass 
			echo Encoding Vimeo 1080p HD Version of "$INFILE"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 10M -pix_fmt yuv420p -vf scale=1920:-1 -c:a libfaac -b:a 320k -ar 44.1k "$VIMFILE" 2>"$VERRLOG"
				# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			echo Moving moov atom.	
			"$qtfaststart" "$VIMFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"	
				
		# Cleanup
			rm "$VERRLOG"			
	done
exit 0
