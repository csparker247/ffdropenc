#!/bin/zsh
PATH=./bin:$PATH

# ffimgdrop
# This is a droplet to encode image sequences to video files.
# This droplet is based off ffdropenc. It uses ffmpeg as its transcoder.

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

# FPS Setup
	fps_options=("24" "25" "29.97" "30" "60")
	fps_count="${#fps_options[@]}"
	
# Collect list of approved file extensions
sequence_exts=`cat seqexts.db`
exts=`cat exts.db`

# Setup qtfaststart
qtfaststart="bin/qtfaststart/qtfaststart"


## Build list of files to encode, using only files from approved extensions list.
sequencelist=()
filelist=()
echo Building file list.
while [ ! -z "$1" ]; do
	if [[ "$1" =~ .*\.($sequence_exts) ]]; then
		inlist="n"
		tempdir=$(dirname "$1")
		tempext=$(basename "$1" | sed 's/.*\.\(.*\)/\1/')
		tempname=$(basename "$1" | sed 's/\(.*\)\..*/\1/')
		collection=$(echo "$tempname" | sed 's/[0-9]*$//')
		chartemp=$(echo "$tempname" | grep -o -m 1 -e '[0-9]*$')
		if [[ "$chartemp" != [0-9]* ]]; then
			shift
			continue
		fi
 		charcount=`printf "%02d" ${#chartemp}`
		new_path="${tempdir}/${collection}%${charcount}d.${tempext}"
		
		for e in "${filelist[@]}"; do
			if [[ "$e" == "$new_path" ]]; then
				inlist="y"
				break
			fi
		done
		
		if [[ "$inlist" == "n" ]]; then
			filelist+=("$new_path")
		fi
	#else
	#OLDIFS=$IFS
	#IFS=$'\n'
	#filelist+=($(find "$1" -type f | grep -e ".*/.*\.\($exts)"))
	#IFS=$OLDIFS
	fi
	shift	
done

echo ${filelist[@]}

# Setup Platypus counter
count=0
args=${#filelist[@]}

# Ask for encoding settings
enc_sets=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output type." --height 150 --items $preset_name`)
select_fps=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output fps:" --height 150 --items $fps_options`)

# Convert selected value to account for lack of 0 index
fps_type="$(echo "scale=1; ${select_fps[2]}+1" | bc)"

# Figure fps from selected option
if [ "${select_fps[1]}" = "2" ]; then
		echo "Encoding cancelled!"
		exit 1
	else
		for j in {1..$fps_count}; do
			if [ $fps_type = $j ]; then
				enc_fps="$fps_options[$fps_type]"
			fi
		done
fi

# Convert selected value to account for lack of 0 index
enc_type="$(echo "scale=1; ${enc_sets[2]}+1" | bc)"

# Check for start approval and call the appropriate encoder
if [ "${enc_sets[1]}" = "2" ]; then
		echo "Encoding cancelled!"
		exit 1
	else
		for j in {1..$preset_count}; do
			if [ $enc_type = $j ]; then
				. presets/$preset_script[$enc_type] 
			fi
		done	
fi	

exit 0