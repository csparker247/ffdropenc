#!/bin/sh

# optname iPad

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
					ERRLOG="$SETNAME"iPad.log
					OUTFILE="$SETNAME"iPad.mp4
				else
					ERRLOG="$SETNAME"_iPad.log
					OUTFILE="$SETNAME"_iPad.mp4
				fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_iPad.log
			OUTFILE="$RAWNAME"_iPad.mp4
			fi
			
		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="1"
						
		# Video pass
			echo "Encoding iPad Version of $INFILE"
			ENCODER="FFMPEG"
			ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -c:v libx264 -b:v 15MB -pix_fmt yuv420p -preset slow -profile:v high -level 41 -maxrate 15MB -bufsize 10MB -vf scale="'if(gt(iw,ih),min(1920,iw),-1)':'if(gt(iw,ih),-1,min(1080,ih))'" -c:a libfdk_aac -ac 2 -b:a 160k -y "$OUTFILE" \
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
