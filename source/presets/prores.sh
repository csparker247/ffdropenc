#!/bin/sh

# optname ProRes 422 (Video Only)

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$index]}")"
			if [[ "$INFILE" =~ .*\.($sequence_exts) ]]; then
				SEQ_OPTS="-f image2 -r $enc_fps"
				SETNAME="$(echo "${filelist[$index]}" | sed 's/%[0-9]*d\..*//')"
				if [[ "$SETNAME" =~ .*(\_|\-|" ") ]]; then
					ERRLOG="$SETNAME"ProRes422.log
					OUTFILE="$SETNAME"ProRes422.mov
				else
					ERRLOG="$SETNAME"_ProRes422.log
					OUTFILE="$SETNAME"_ProRes422.mov
				fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_ProRes422.log
			OUTFILE="$RAWNAME"_ProRes422.mov
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding ProRes 422 (Video Only) Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -c:v prores -y "$OUTFILE" \
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
