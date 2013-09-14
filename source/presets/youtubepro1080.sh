#!/bin/sh

# optname YouTube 1080p HD (Professional)

# Encode each file
for (( i=1; i<=${args}; i++ )); do
		SEQ_OPTS=""
		index=$(expr $i - 1)
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$index]}")"
			if [[ "$INFILE" =~ .*\.($sequence_exts) ]]; then
				SEQ_OPTS="-f image2 -r $enc_fps"
				SETNAME="$(echo "${filelist[$index]}" | sed 's/%[0-9]*d\..*//')"
				#if [[ $SETNAME != .*(\_|\-|" ") ]]; then
				#	ERRLOG="$SETNAME"_H264.log
				#	OUTFILE="$SETNAME"_H264.mp4
				#else
					TWOPASS="$SETNAME"_2pass
					ERRLOG="$SETNAME"_YouTube1080.log
					OUTFILE="$SETNAME"_YouTube1080.mp4
				#fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="$RAWNAME"_2pass
			ERRLOG="$RAWNAME"_YouTube1080.log
			OUTFILE="$RAWNAME"_YouTube1080.mp4
			fi

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="2"

		# Video Pass #1
			echo "Encoding 1st Pass, YouTube 1080p HD (Professional) Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 1 -passlogfile "$TWOPASS" -c:v libx264 -b:v 45M -maxrate 55M -bufsize 3M -pix_fmt yuv420p -vf scale="'if(gt(iw,ih),min(1920,iw),-1)':'if(gt(iw,ih),-1,min(1080,ih))'" -an -y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		
		# Video Pass #2	
			echo "Encoding 2nd Pass, YouTube 1080p HD (Professional) Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 2 -passlogfile "$TWOPASS" -c:v libx264 -b:v 45M -maxrate 55M -bufsize 3M -pix_fmt yuv420p -vf scale="'if(gt(iw,ih),min(1920,iw),-1)':'if(gt(iw,ih),-1,min(1080,ih))'" -c:a libfdk_aac -profile:a aac_low -b:a 320k -ar 48k -y "$OUTFILE" \
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

