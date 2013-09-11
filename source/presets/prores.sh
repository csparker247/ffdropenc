#!/bin/sh

# optname ProRes 422

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			SETNAME="$(echo "${filelist[$i]}" | sed 's/%[0-9]*d\..*//')"
			if [[ $SETNAME != *(_|-) ]]; then
				ERRLOG="$SETNAME"_ProRes.log
				OUTFILE="$SETNAME"_ProRes.mov
			else
				ERRLOG="$SETNAME"ProRes.log
				OUTFILE="$SETNAME"ProRes.mov
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding ProRes 422 Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -f image2 -r "$enc_fps" -i "${filelist[$i]}" -c:v prores -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+1)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		# Cleanup
			rm "$ERRLOG"			
	done
exit 0
