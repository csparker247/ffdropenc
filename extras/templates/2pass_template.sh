#!/bin/sh

# optname [TWO-PASS TEMPLATE]

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
					TWOPASS="$SETNAME"2pass
					ERRLOG="$SETNAME"TEMPLATE-2pass.log
					OUTFILE="$SETNAME"TEMPLATE-2pass.mp4
				else
					TWOPASS="$SETNAME"_2pass
					ERRLOG="$SETNAME"_TEMPLATE-2pass.log
					OUTFILE="$SETNAME"_TEMPLATE-2pass.mp4
				fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="$RAWNAME"_2pass
			ERRLOG="$RAWNAME"_TEMPLATE-2pass.log
			OUTFILE="$RAWNAME"_TEMPLATE-2pass.mp4
			fi

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="2"
						
		# Video Pass #1
			echo "Encoding 1st Pass, TEMPLATE Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -pix_fmt yuv420p -vf "scale=iw*sar:ih, scale='if(gt(iw,ih),min(1280,iw),-1)':'if(gt(iw,ih),-1,min(720,ih))'" -an -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		
		# Video Pass #2	
			echo "Encoding 2nd Pass, TEMPLATE Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 2 -passlogfile "$TWOPASS" -c:v libx264 -b:v 5M -maxrate 5M -bufsize 2M -pix_fmt yuv420p -vf "scale=iw*sar:ih, scale='if(gt(iw,ih),min(1280,iw),-1)':'if(gt(iw,ih),-1,min(720,ih))'" -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
			
		echo Moving moov atom.	
		"$qtfaststart" "$OUTFILE"
	
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
								
		# Cleanup
			rm "$ERRLOG"
			rm "$TWOPASS"-0.log
			rm "$TWOPASS"-0.log.mbtree	
	done

