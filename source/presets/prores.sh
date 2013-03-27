#!/bin/sh

# optname ProRes 422 (Video Only)

# Encode each file for YouTube
for (( i=1; i<=${args}; i++ ));
	do
		# Remove the extension and make filenames for logs and output.
			INFILE="$(basename "${filelist[$i]}")"
			RAWNAME="$(echo "${filelist[$i]}" | sed 's/\(.*\)\..*/\1/')"
			ERRLOG="$RAWNAME"_ProRes.log
			OUTFILE="$RAWNAME"_ProRes.mov

				
		# ProRes Pass
			echo "Encoding ProRes 422 (Video Only) Version of $INFILE"
			ffmpeg -i "${filelist[$i]}" -c:v prores "$OUTFILE" 2>"$ERRLOG"
				# Progress update
				count=$(echo "scale=3; $count+0.8" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
			echo Moving moov atom.	
			"$qtfaststart" "$OUTFILE"
				# Progress update
				count=$(echo "scale=3; $count+0.2" | bc)
				PROG=$(echo "scale=3; ($count/$args)*100.0" | bc)
				echo PROGRESS:"$PROG"
				
		# Cleanup
			rm "$ERRLOG"

		done