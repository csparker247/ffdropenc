#!/bin/sh

# optname Bluray - 1080p H.264, AC-3

# Encode each file

echo PROGRESS:5

for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			AUDERRLOG="$RAWNAME"_AUDIO.log
			OUTFILE="$RAWNAME"_BD.264
			AUDIOFILE="$RAWNAME"_AUDIO.ac3

				
		# Bluray Encode
			# First pass 
			echo "Encoding 1st Pass, Bluray 1080p of $INFILE"
			x264 --no-progress --quiet --log-level none --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 1 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}"
			# Progress update
				count=$(echo "scale=3; $count+0.33" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			# Second Pass
			echo "Encoding 2nd Pass, Bluray 1080p of $INFILE"
			x264 --no-progress --quiet --log-level none --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 2 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}"
			# Progress update
				count=$(echo "scale=3; $count+0.33" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			# Audio Pass
			echo "Encoding AC-3 Audio for $INFILE"
			ffmpeg -i "${filelist[$i]}" -b:a ac3 -b:a 640k -ar 48000 "$AUDIOFILE" 2>"$AUDERRLOG"
			# Progress update
				count=$(echo "scale=3; $count+0.34" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
			
		# Cleanup
			rm "$LOGNAME"
			rm "$LOGNAME".mbtree
			rm "$AUDERRLOG"

		done