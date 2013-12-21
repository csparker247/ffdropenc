#!/bin/sh

# optname MOV Remux

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$index]}")"
			if [[ "$useNewOutput" == "1" ]]; then
				OUTPATH="$newOutputPath"
			else
				OUTPATH="$(dirname "${filelist[$index]}")"
			fi
			if [[ "$INFILE" =~ .*\.($sequence_exts) ]]; then
				SEQ_OPTS="-f image2 -r $enc_fps"
				SETNAME="$(echo "$INFILE" | sed 's/%[0-9]*d\..*//')"
				if [[ "$SETNAME" =~ .*(\_|\-|" ") ]]; then
					ERRLOG="${OUTPATH}/${SETNAME}"Remux.log
					OUTFILE="${OUTPATH}/${SETNAME}"Remux.mov
				else
					ERRLOG="${OUTPATH}/${SETNAME}"_Remux.log
					OUTFILE="${OUTPATH}/${SETNAME}"_Remux.mov
				fi
			else
				RAWNAME="$(echo "$INFILE" | sed 's/\(.*\)\..*/\1/')"
				ERRLOG="${OUTPATH}/${RAWNAME}"_Remux.log
				OUTFILE="${OUTPATH}/${RAWNAME}"_Remux.mov
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding MOV Remux Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -c:v copy -c:a copy -y "$OUTFILE" \
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
