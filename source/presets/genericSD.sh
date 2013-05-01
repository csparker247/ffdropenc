#!/bin/sh

# optname Generic SD (480px Width)

# Encode each file for Vimeo
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			ERRLOG="$RAWNAME"_genericSD-debug.log
			GENFILE="$RAWNAME"_genericSD.mp4
			
		# Get video duration in frames
			duration=$(ffmpeg -i "${filelist[$i]}" 2>&1 | sed -n "s/.* Duration: \([^,]*\), start: .*/\1/p")
			fps=$(ffmpeg -i "${filelist[$i]}" 2>&1 | sed -n "s/.*, \(.*\) tbr.*/\1/p")
			hours=$(echo $duration | cut -d":" -f1)
			minutes=$(echo $duration | cut -d":" -f2)
			seconds=$(echo $duration | cut -d":" -f3)
			FRAMES=$(echo "($hours*3600+$minutes*60+$seconds)*$fps" | bc | cut -d"." -f1)
		
		# Video pass 
			echo "Encoding Generic SD (480px Width) of $INFILE"
			ffmpeg -i "${filelist[$i]}" -c:v libx264 -b:v 2500K -pix_fmt yuv420p -profile:v baseline -vf scale=480:-1 -c:a libfdk_aac -b:a 192k -ar 44.1k -y "$GENFILE" 2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		# Wait for progress.sh to end
			# Move the faststart atom
			echo Moving moov atom.	
			"$qtfaststart" "$GENFILE"
				
		# Update progress
			count=$(echo "scale=3; ($count+1)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		# Cleanup
			rm "$ERRLOG"			
	done
exit 0
