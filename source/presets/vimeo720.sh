#!/bin/sh

# optname Vimeo 720p HD

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
					ERRLOG="$SETNAME"Vimeo720.log
					OUTFILE="$SETNAME"Vimeo720.mp4
				else
					ERRLOG="$SETNAME"_Vimeo720.log
					OUTFILE="$SETNAME"_Vimeo720.mp4
				fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_Vimeo720.log
			OUTFILE="$RAWNAME"_Vimeo720.mp4
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding Vimeo 720p HD Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -c:v libx264 -b:v 5M -pix_fmt yuv420p -profile:v baseline -vf scale="'if(gt(iw,ih),min(1280,iw),-1)':'if(gt(iw,ih),-1,min(720,ih))'" -c:a libfdk_aac -b:a 320k -ar 44.1k -y "$OUTFILE" \
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
			rm "$ERRLOG"			
	done
exit 0
