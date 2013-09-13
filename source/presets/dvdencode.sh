#!/bin/sh

# optname DVD - Widescreen, MPEG-2, AC-3

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
					ERRLOG="$SETNAME"_DVD.log
					OUTFILE="$SETNAME"_DVD.m2v
					AUDIOFILE="$SETNAME"_DVD_AUDIO.ac3
				#fi
			else
			RAWNAME="$(echo "${filelist[$index]}" | sed 's/\(.*\)\..*/\1/')"
			TWOPASS="$RAWNAME"_2pass
			ERRLOG="$RAWNAME"_DVD.log
			OUTFILE="$RAWNAME"_DVD.m2v
			AUDIOFILE="$RAWNAME"_DVD_AUDIO.ac3
			fi


		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="3"
				
		# DVD Encode
			# First pass 
				echo "Encoding 1st Pass, DVD - Widescreen of $INFILE"
				ENCODER="FFMPEG"
				ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 1 -passlogfile "$TWOPASS" -pix_fmt yuv420p -s 720x480 -r 29.97 -g 18 -b:v 9M -maxrate 9M -minrate 0 -bufsize 1835008 -packetsize 2048 -muxrate 10080000 -vf scale="'if(gt(iw,ih),720,-1)':'if(gt(iw,ih),-1,480)'" -an -y "$OUTFILE" \
				2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
		
			# Track encoding progress	
				. progress.sh
		
			# Update progress
				count=$(echo "scale=3; ($count+0.33)" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
			# Second Pass
				echo "Encoding 2nd Pass, DVD - Widescreen of $INFILE"
				ENCODER="FFMPEG"
				ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -pass 2 -passlogfile "$TWOPASS" -pix_fmt yuv420p -s 720x480 -r 29.97 -g 18 -b:v 9M -maxrate 9M -minrate 0 -bufsize 1835008 -packetsize 2048 -muxrate 10080000 -vf scale="'if(gt(iw,ih),720,-1)':'if(gt(iw,ih),-1,480)'" -an -y "$OUTFILE" \
				2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
			# Track encoding progress	
				. progress.sh
		
			# Update progress
				count=$(echo "scale=3; ($count+0.33)" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
			# Audio Pass
				echo "Encoding AC-3 Audio for $INFILE"
				ENCODER="FFMPEG"
				ffmpeg $(echo $SEQ_OPTS) -i "${filelist[$index]}" -b:a ac3 -b:a 448k -ar 48000 "$AUDIOFILE" \
				2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
			# Track encoding progress	
				. progress.sh
				
			if [[ ! -f "$AUDIOFILE" ]]; then
					echo "--------"
					echo "Warning: Audio file not created for ${INFILE}. Does it have audio?"
					echo "--------"
			fi
		
			# Update progress
				count=$(echo "scale=3; ($count+0.34)" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
		# Cleanup
			rm "$ERRLOG"
			rm "$TWOPASS"-0.log

		done