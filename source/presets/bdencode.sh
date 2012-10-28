#!/bin/sh

# optname Bluray 1080p (Video Only)

# Encode each file for YouTube
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			LOGNAME="$RAWNAME"_2pass.log
			OUTFILE="$RAWNAME"_BD.264

				
		# Bluray Encode
			# First pass 
			echo "Encoding 1st Pass, Bluray 1080p (Video Only) of $INFILE"
			x264 --no-progress --quiet --log-level none --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 1 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}"
			# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
		# Second Pass
			echo "Encoding 2nd Pass, Bluray 1080p (Video Only) of $INFILE"
			x264 --no-progress --quiet --log-level none --input-csp yuv422p --bitrate 39000 --preset veryslow --tune film --fps 29.97 --input-res 1920x1080 --bluray-compat --pic-struct --vbv-maxrate 40000 --vbv-bufsize 30000 --level 4.1 --keyint 30 --slices 4 --fake-interlaced --colorprim bt709 --transfer bt709 --colormatrix bt709 --sar 1:1 --pass 2 --stats "$LOGNAME" -o "$OUTFILE" "${filelist[$i]}"
			# Progress update
				count=$(echo "scale=3; $count+0.5" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
		
		# Cleanup
			rm "$LOGNAME"
			rm "$LOGNAME".mbtree

		done