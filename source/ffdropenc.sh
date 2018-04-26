#!/bin/bash
PATH=./bin:$PATH

# ffdropenc
# Version 1.8.2
# This is a droplet to transcode videos and image sequences to preset video types.
# It uses ffmpeg as its transcoder.

# Quit if no files/arguments given
if [ $# -eq 0 ]; then
	exit 0
fi

# Program setup

# Build encoding options list
preset_script=()
preset_name=()
preset_count="0"

for i in presets/*.sh; do
	preset_type=$(sed -n /optname/p "$i" | sed 's/# optname //')
	if [[ "$preset_type" != "" ]]; then
		preset_script+=( "$(basename "$i")" )
		preset_name+=( "$preset_type" )
	fi
done
# Count number of presets
preset_count="${#preset_name[@]}"

# FPS Setup
fps_options=( "24" "25" "29.97" "30" "60" "120" )
fps_count="${#fps_options[@]}"

# Collect list of approved file extensions
filters=`cat resources/filters.db`
sequence_exts=`cat resources/seqexts.db`
mov_exts=`cat resources/movexts.db`

# Setup programs, import global functions
qtfaststart="bin/qtfaststart/qtfaststart"
cocoaDialog="bin/cocoaDialog.app/Contents/MacOS/cocoaDialog"
. functions.sh

# Build list of files to encode, using only files from approved extensions list.
OLDIFS=$IFS
IFS=$'\n'
argument_paths=( $@ )
IFS=$OLDIFS
argument_count=$(expr ${#argument_paths[@]} - 1)
for (( i=0; i<=${argument_count}; i++ )); do
	OLDIFS=$IFS
	IFS=$'\n'
	argument_files+=( $(find "${argument_paths[$i]}" -type f | grep -e ".*/.*\.\($filters)") )
	IFS=$OLDIFS
done

filelist=()
has_sequences="n"

echo "Building file list..."
argument_count=$(expr ${#argument_files[@]} - 1)
for (( i=0; i<=${argument_count}; i++ )); do
    inputName=${argument_files[$i]}
	inlist="n"

	if [[ $inputName =~ .*\.($sequence_exts) ]]; then
		has_sequences="y"
		tempdir=$(dirname "$inputName")
		tempext=$(basename "$inputName" | sed 's/.*\.\(.*\)/\1/')
		tempname=$(basename "$inputName" | sed 's/\(.*\)\..*/\1/')
		collection=$(echo "$tempname" | sed 's/[0-9]*$//')
		numtemp=$(echo "$tempname" | grep -o -m 1 -e '[0-9]*$')

		if [[ "$numtemp" != [0-9]* ]]; then
			continue
		fi

		charcount=`printf "%02d" ${#numtemp}`
		outputName="${tempdir}/${collection}%${charcount}d.${tempext}"
	elif [[ "$inputName" =~ .*\.($mov_exts) ]]; then
		outputName="$inputName"
	fi

	args=${#filelist[@]}
	if [[ $args != "0" ]]; then
		for (( e=1; e<=${args}; e++ )); do
			tempE=$(expr $e - 1)
			if [[ "${filelist[$tempE]}" == "$outputName" ]]; then
				inlist="y"
				break
			fi
		done
	fi

	if [[ "$inlist" == "n" ]]; then
		filelist+=( "$outputName" )
	fi

done
unset argument_files
unset tempE

# Setup Platypus counter
count=0
args=${#filelist[@]}

if [[ "$args" == "0" || "${filelist[@]}" == "" ]]; then
	echo "No supported file types in batch list."
	exit 0
fi

# Compile total number of frames to be encoded
TOTALFRAMES="0"
FINISHEDFRAMES="0"
for (( i=1; i<=${args}; i++ )); do
	FILE="$(echo "${filelist[$index]}")"
	tempFRAMES=$(getLength "$FILE")
	TOTALFRAMES=$(echo "$TOTALFRAMES + $tempFRAMES" | bc)
done

# Ask for encoding settings
OLDIFS=$IFS
IFS=$'\n'
enc_sets=(`$cocoaDialog standard-dropdown --title "ffdropenc" --text "Select output type." --height 150 --items $(for i in ${preset_name[@]}; do echo "$i"; done)`)
IFS=$OLDIFS
if [[ "${enc_sets[0]}" == "2" ]]; then
		echo "Encoding cancelled!"
		exit 1
fi

if [[ "$has_sequences" == "y" ]]; then
	select_fps=(`$cocoaDialog standard-dropdown --title "ffdropenc" --text "Select output fps." --height 150 --items "${fps_options[@]}"`)
	# Figure fps from selected option
	fps_type=${select_fps[1]}
	if [[ "${select_fps[0]}" == "2" ]]; then
			echo "Encoding cancelled!"
			exit 1
		else
			for j in $(eval echo "{0..$(expr $fps_count - 1)}"); do
				if [[ "$fps_type" == $j ]]; then
					enc_fps="${fps_options[$j]}"
					break
				fi
			done
	fi
fi

# Ask for Custom Output Path
useNewOutput=`$cocoaDialog yesno-msgbox --no-cancel --title "ffdropenc" --text "Use a custom output folder?" --informative-text "NOTE: All files will be saved to this folder." --icon folder`
if [[ "$useNewOutput" == "1" ]]; then
	newOutputPath=`$cocoaDialog fileselect --select-only-directories --title "ffdropenc" --text "Select an output folder:"`
	if [[ ! -n "$newOutputPath" ]]; then  ### if $rv has a non-zero length
		echo "No output path selected. Encoding cancelled!"
		exit 1
	fi
elif [[ "$useNewOutput" == "2" ]]; then
	echo "Outputting to path of source file."
fi

# Check for start approval and call the appropriate encoder
enc_type="${enc_sets[1]}"
for j in $(eval echo "{0..$(expr $preset_count - 1)}"); do
	if [[ "$enc_type" == $j ]]; then
		. presets/${preset_script[$j]}
		break
	fi
done

exit 0
