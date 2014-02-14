#!/bin/sh

# Global Functions

getLength () {
	# Get video duration in frames
	duration=$(ffmpeg -i "$1" 2>&1 | sed -n "s/.* Duration: \([^,]*\), start: .*/\1/p")
	fps=$(ffmpeg -i "$1" 2>&1 | sed -n "s/.*, \(.*\) tbr.*/\1/p")
	hours=$(echo $duration | cut -d":" -f1)
	minutes=$(echo $duration | cut -d":" -f2)
	seconds=$(echo $duration | cut -d":" -f3)
	FRAMES=$(echo "($hours*3600+$minutes*60+$seconds)*$fps" | bc | cut -d"." -f1)
}


setOutputs () {
	# Remove the extension and make filenames for logs and output.
	INPUT_NAME="$(basename "$1")"
	# Set output path
	if [[ "$useNewOutput" == "1" ]]; then
		OUTPATH="$newOutputPath"
	else
		OUTPATH="$(dirname "$1")"
	fi

	# Remove numbers & extension if sequence, only extension otherwise
	if [[ "$INPUT_NAME" =~ .*\.($sequence_exts) ]]; then
		SEQ_OPTS="-f image2 -r $enc_fps"
		X_OPTS="--fps $enc_fps"
		RAWNAME="$(echo "$INPUT_NAME" | sed 's/%[0-9]*d\..*//')"
	else
		RAWNAME="$(echo "$INPUT_NAME" | sed 's/\(.*\)\..*/\1/')"
	fi

	# Add underscore if there's not a separator in RAWNAME
	[[ "$RAWNAME" != *_ ]] && [[ "$RAWNAME" != *- ]] && [[ "$RAWNAME" != *" " ]] && RAWNAME="$RAWNAME"_

	ERRLOG="${OUTPATH}/${RAWNAME}${VSUFFIX}.log"
	TWOPASSLOG="${OUTPATH}/${RAWNAME}${TPL_SUFFIX}"
	OUTFILE="${OUTPATH}/${RAWNAME}${VSUFFIX}.${VEXTENSION}"
	AUDIOFILE="${OUTPATH}/${RAWNAME}${ASUFFIX}.${AEXTENSION}"
}