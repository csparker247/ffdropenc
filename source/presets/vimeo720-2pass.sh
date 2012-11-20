#!/bin/sh

# optname Vimeo 720p HD (2-Pass)

# Encode each file for Vimeo
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="${filelist[$i]}"vim720
			ERRLOG="$RAWNAME"_VIM720-2pass-debug.log
			OUTFILE="$RAWNAME"_VIM720-2pass.mp4

				
		# YouTube Pass
			echo "Encoding 1st Pass, Vimeo 720p HD Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -pix_fmt yuv420p -vf scale=1280:-1 -an -y "$OUTFILE" 2>"$ERRLOG"1
				# Progress update
				count=$(echo "scale=3; $count+0.4" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo "Encoding 2nd Pass, Vimeo 720p HD Version of Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -pass 2 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -maxrate 5M -bufsize 2M -pix_fmt yuv420p -vf scale=1280:-1 -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" 2>"$ERRLOG"2
				# Progress update
				count=$(echo "scale=3; $count+0.4" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo Moving moov atom.	
			"$qtfaststart" "$OUTFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.2" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
		# Cleanup
			rm "$ERRLOG"1
			rm "$ERRLOG"2
			rm "$TWOPASS"-0.log
			rm "$TWOPASS"-0.log.mbtree	
	done

