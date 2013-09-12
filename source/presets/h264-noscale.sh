#!/bin/sh

# optname H.264 Generic (No Scale)

# Encode each file
for (( i=1; i<=${args}; i++ ));
	do
		SEQ_OPTS=""
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			if [[ "$INFILE" =~ .*\.($sequence_exts) ]]; then
				SEQ_OPTS="-f image2 -r $enc_fps"
				SETNAME="$(echo "${filelist[$i]}" | sed 's/%[0-9]*d\..*//')"
				if [[ $SETNAME != *(_|-|" ") ]]; then
					ERRLOG="$SETNAME"_H264.log
					OUTFILE="$SETNAME"_H264.mp4
				else
					ERRLOG="$SETNAME"H264.log
					OUTFILE="$SETNAME"H264.mp4
				fi
			else
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_H264.log
			OUTFILE="$RAWNAME"_H264.mp4
			fi

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding H.264 Generic (No Scale) Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$i]}" -c:v libx264 -b:v 10M -maxrate 10M -bufsize 3M -pix_fmt yuv420p -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Move the faststart atom
		echo Moving moov atom.	
		"$qtfaststart" "$OUTFILE"
				
		# Update progress
			count=$(echo "scale=3; ($count+1)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		# Cleanup
			#rm "$ERRLOG"			
	done
exit 0
