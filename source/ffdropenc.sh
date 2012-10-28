#!/bin/zsh
PATH=./bin:$PATH

# ffdropenc
# This is a droplet to encode videos for upload to YouTube and Vimeo.
# This droplet is based off the Bluray Encoding Droplet. It uses ffmpeg as its transcoder.

# Quit if no files/arguments given
if [ $# -eq 0 ]; then
	exit 0
fi

## Program setup
# Build encoding options list
	preset_script=()
	preset_name=()
	preset_count="0"
	
	for i in presets/*.sh; do
		preset_type=$(sed -n /optname/p "$i" | sed 's/# optname //')
		if [ ! $preset_type = "" ]; then
			preset_script+="$(basename "$i")"
			preset_name+="$preset_type"
		fi
	done
	# Count number of presets
	preset_count="${#preset_name[@]}"
	
# Collect list of approved file extensions
exts=`cat exts.db`

# Setup qtfaststart
qtfaststart="bin/qtfaststart/qtfaststart"


## Build list of files to encode, using only files from approved extensions list.
filelist=()
echo Building file list.
while [ ! -z "$1" ]
do
		OLDIFS=$IFS
		IFS=$'\n'
		filelist+=($(find "$1" -type f | grep -e ".*/.*\.\($exts)"))
		IFS=$OLDIFS
		shift
done	

# Setup Platypus counter
count=0
args=${#filelist[@]}
# For accurate multi-output counter: PROG=($count/($args*${#enc_sets[@]}))*100.000

# Ask for encoding settings
enc_sets=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffdropenc" --text "Select output type." --height 150 --items $preset_name`)
	
# Convert selected value to account for lack of 0 index
enc_type="$(echo "scale=1; ${enc_sets[2]}+1" | bc)"

# Check for start approval and call the appropriate encoder
echo PROGRESS:5
if [ "${enc_sets[1]}" = "2" ]; then
		echo "Encoding cancelled!"
		exit 1
	else
		# echo PROGRESS:5
		for j in {1..$preset_count}; do
			if [ $enc_type = $j ]; then
				. presets/$preset_script[$enc_type] 
			fi
		done	
fi	

exit 0