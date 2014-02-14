#!/bin/sh

# optname Small MP4 720p HD (Deinterlaced)

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
					ERRLOG="${OUTPATH}/${SETNAME}"H264Deint.log
					OUTFILE="${OUTPATH}/${SETNAME}"H264Deint.mp4
				else
					ERRLOG="${OUTPATH}/${SETNAME}"_H264Deint.log
					OUTFILE="${OUTPATH}/${SETNAME}"_H264Deint.mp4
				fi
			else
				RAWNAME="$(echo "$INFILE" | sed 's/\(.*\)\..*/\1/')"
				ERRLOG="${OUTPATH}/${RAWNAME}"_H264Deint.log
				OUTFILE="${OUTPATH}/${RAWNAME}"_H264Deint.mp4
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding Small MP4 720p HD Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" \
			-c:v libx264 -crf 22 -maxrate 3M -bufsize 6M -pix_fmt yuv420p -profile:v high -level 42 -vf "scale=iw*sar:ih, setsar=1/1, yadif, scale='if(gt(iw,ih),min(1280,ceil(iw/2)*2),trunc(oh*a/2)*2)':'if(gt(iw,ih),trunc(ow/a/2)*2,min(720,ceil(ih/2)*2))'" -movflags faststart \
			-c:a libfdk_aac -b:a 192k \
			-y "$OUTFILE" \
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
