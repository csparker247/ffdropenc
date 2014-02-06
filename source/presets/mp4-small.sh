#!/bin/sh

# optname Small MP4 720p HD (2-Pass, Very Slow Encode)

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
					TWOPASS="${OUTPATH}/${SETNAME}"2pass
					ERRLOG="${OUTPATH}/${SETNAME}"Small720.log
					OUTFILE="${OUTPATH}/${SETNAME}"Small720.mp4
				else
					TWOPASS="${OUTPATH}/${SETNAME}"_2pass
					ERRLOG="${OUTPATH}/${SETNAME}"_Small720.log
					OUTFILE="${OUTPATH}/${SETNAME}"_Small720.mp4
				fi
			else
			RAWNAME="$(echo "$INFILE" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="${OUTPATH}/${RAWNAME}"_2pass
			ERRLOG="${OUTPATH}/${RAWNAME}"_Small720.log
			OUTFILE="${OUTPATH}/${RAWNAME}"_Small720.mp4
			fi

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="2"
						
		# Video Pass #1
			echo "Encoding 1st Pass, Small MP4 720p HD Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 1 -passlogfile "$TWOPASS" \
			-c:v libx264 -b:v 2M -pix_fmt yuv420p -profile:v high -level 42 -preset veryslow -vf "scale=iw*sar:ih, scale='if(gt(iw,ih),min(1280,iw),-1)':'if(gt(iw,ih),-1,min(720,ih))'" \
			-an \
			-y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
		
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
		
		# Video Pass #2	
			echo "Encoding 2nd Pass, Small MP4 720p HD Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 2 -passlogfile "$TWOPASS" \
			-c:v libx264 -b:v 2M -maxrate 5M -bufsize 2M -pix_fmt yuv420p -profile:v high -level 42 -preset veryslow -vf "scale=iw*sar:ih, scale='if(gt(iw,ih),min(1280,iw),-1)':'if(gt(iw,ih),-1,min(720,ih))'" -movflags faststart \
			-c:a libfdk_aac -b:a 192k \
			-y "$OUTFILE" \
			2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
		# Track encoding progress	
			. progress.sh
	
		# Update progress
			count=$(echo "scale=3; ($count+0.5)" | bc)
			PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
			echo PROGRESS:"$PROG"
								
		# Cleanup
			rm "$ERRLOG"
			rm "$TWOPASS"-0.log
			rm "$TWOPASS"-0.log.mbtree	
	done

