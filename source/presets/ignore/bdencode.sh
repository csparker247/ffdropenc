#!/bin/sh

# optname Bluray - 1080p H.264, AC-3

# Encode each file

for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			ERRLOG="$RAWNAME"_BD.log
			OUTFILE="$RAWNAME"_BD_VIDEO.264
			AUDIOFILE="$RAWNAME"_BD_AUDIO.ac3

		# Type of encode: 1 = single pass, 2 = two-pass, 3 = three-pass/two-pass+audio, etc.
			NUM_PASSES="3"
				
		# Bluray Encode
			# First pass 
				echo "Encoding 1st Pass, Bluray 1080p of $INFILE"
				ENCODER="X264"
				x264 --quiet --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 1 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}" \
				2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
		
			# Track encoding progress	
				. progress.sh
		
			# Update progress
				count=$(echo "scale=3; ($count+0.33)" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
			# Second Pass
				echo "Encoding 2nd Pass, Bluray 1080p of $INFILE"
				ENCODER="X264"
				x264 --quiet --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 2 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}" \
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
				ffmpeg -i "${filelist[$i]}" -b:a ac3 -b:a 640k -ar 48000 "$AUDIOFILE" \
				2>&1 | awk '1;{fflush()}' RS='\r\n'>"$ERRLOG" &
			
			# Track encoding progress	
				. progress.sh
		
			# Update progress
				count=$(echo "scale=3; ($count+0.34)" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
		# Cleanup
			rm "$ERRLOG"
			rm "$LOGNAME"
			rm "$LOGNAME".mbtree

		done