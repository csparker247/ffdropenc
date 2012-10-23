#!/bin/sh
PATH=$PATH:./bin

# Encode each file for Vimeo
for (( i=0; i<${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			VERRLOG="$RAWNAME"_vim720-debug.log
			VIMFILE="$RAWNAME"_VIM720.mp4

		# Vimeo pass 
			echo Encoding Vimeo 720p HD Version of "$INFILE"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 5M -pix_fmt yuv420p -s 1280x720 -c:a libfaac -b:a 320k -ar 44.1k "$VIMFILE" 2>"$VERRLOG"
				# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			echo Moving moov atom.	
			qtfaststart "$VIMFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"	
				
		# Cleanup
			rm "$VERRLOG"			
	done
exit 0
