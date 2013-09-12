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
	has_sequences="n"
	
	for i in presets/*.sh; do
		preset_type=$(sed -n /optname/p "$i" | sed 's/# optname //')
		if [[ "$preset_type" != "" ]]; then
			preset_script+="$(basename "$i")"
			preset_name+=$preset_type
		fi
	done
	# Count number of presets
	preset_count="${#preset_name[@]}"

# FPS Setup
	fps_options=("24" "25" "29.97" "30" "60")
	fps_count="${#fps_options[@]}"


# Collect list of approved file extensions
filters=`cat filters.db`
sequence_exts=`cat seqexts.db`
mov_exts=`cat movexts.db`

# Setup qtfaststart
qtfaststart="bin/qtfaststart/qtfaststart"

## Build list of files to encode, using only files from approved extensions list.
argument_files=()
filelist=()

echo "Building file list."
for argument in $@; do
	OLDIFS=$IFS
	IFS=$'\n'
	argument_files+=($(find "$argument" -type f | grep -e ".*/.*\.\($filters)"))
	IFS=$OLDIFS
done

for thisfile in $argument_files; do
	echo $thisfile
	if [[ "$thisfile" =~ .*\.($sequence_exts) ]]; then
		has_sequences="y"
		inlist="n"
		tempdir=$(dirname "$thisfile")
		tempext=$(basename "$thisfile" | sed 's/.*\.\(.*\)/\1/')
		tempname=$(basename "$thisfile" | sed 's/\(.*\)\..*/\1/')
		collection=$(echo "$tempname" | sed 's/[0-9]*$//')
		chartemp=$(echo "$tempname" | grep -o -m 1 -e '[0-9]*$')
		
		if [[ "$chartemp" != [0-9]* ]]; then
			continue
		fi
		
		charcount=`printf "%02d" ${#chartemp}`
		new_path="${tempdir}/${collection}%${charcount}d.${tempext}"
	
		for e in $filelist; do
			if [[ "$e" == "$new_path" ]]; then
				inlist="y"
				break
			fi
		done
	
		if [[ "$inlist" == "n" ]]; then
			OLDIFS=$IFS
			IFS=$'\n'
			filelist+=("$new_path")
			IFS=$OLDIFS
		fi
		
	elif [[ "$thisfile" =~ .*\.($mov_exts) ]]; then
		OLDIFS=$IFS
		IFS=$'\n'
		filelist+=("$thisfile")
		IFS=$OLDIFS
	fi
done

echo "I made it!"
unset argument_files

# Setup Platypus counter
count=0
args=${#filelist[@]}

if [[ "$args" == "0" ]]; then
	echo "No supported file types in batch list."
	exit 0
fi

# Ask for encoding settings

enc_sets=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output type." --height 150 --items $preset_name`)
if [[ "$has_sequences" == "y" ]]; then
	select_fps=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output fps." --height 150 --items $fps_options`)
	# Convert selected value to account for lack of 0 index
		fps_type="$(echo "scale=1; ${select_fps[2]}+1" | bc)"

	# Figure fps from selected option
	if [[ "${select_fps[1]}" == "2" ]]; then
			echo "Encoding cancelled!"
			exit 1
		else
			for j in {1..$fps_count}; do
				if [[ $fps_type == $j ]]; then
					enc_fps="$fps_options[$fps_type]"
				fi
			done
	fi
fi

# Convert selected value to account for lack of 0 index
enc_type="$(echo "scale=1; ${enc_sets[2]}+1" | bc)"

# Check for start approval and call the appropriate encoder
if [[ "${enc_sets[1]}" == "2" ]]; then
		echo "Encoding cancelled!"
		exit 1
	else
		for j in {1..$preset_count}; do
			if [[ $enc_type == $j ]]; then
				. presets/$preset_script[$enc_type] 
			fi
		done	
fi	

exit 0