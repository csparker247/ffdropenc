#!/bin/sh

# optname MOV Remux

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			SETNAME="$(echo "${filelist[$i]}" | sed 's/%[0-9]*d\..*//')"
			if [[ $SETNAME != *(_|-) ]]; then
				ERRLOG="$SETNAME"_REMUX.log
				OUTFILE="$SETNAME"_REMUX.mov
			else
				ERRLOG="$SETNAME"REMUX.log
				OUTFILE="$SETNAME"REMUX.mov
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding MOV Remux Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg -f image2 -r "$enc_fps" -i "${filelist[$i]}" -c:v copy -y "$OUTFILE" \
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
