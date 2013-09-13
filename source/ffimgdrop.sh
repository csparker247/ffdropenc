#!/bin/sh
PATH=./bin:$PATH

# ffimgdrop
# This is a droplet to encode image sequences to video files.
# This droplet is based off ffdropenc. It uses ffmpeg as its transcoder.

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
fps_options=( "24" "25" "29.97" "30" "60" )
fps_count="${#fps_options[@]}"

# Collect list of approved file extensions
filters=`cat filters.db`
sequence_exts=`cat seqexts.db`
mov_exts=`cat movexts.db`

# Setup qtfaststart
qtfaststart="bin/qtfaststart/qtfaststart"

# Build list of files to encode, using only files from approved extensions list.
argument_paths=( $@ )
argument_count=$(expr ${#argument_paths[@]} - 1)
for (( i=0; i<=${argument_count}; i++ )); do
	OLDIFS=$IFS
	IFS=$'\n'
	argument_files+=( $(find "${argument_paths[$i]}" -type f | grep -e ".*/.*\.\($filters)") )
	IFS=$OLDIFS
done

filelist=()
has_sequences="n"

echo "Building file list."
argument_count=$(expr ${#argument_files[@]} - 1)
for (( i=0; i<=${argument_count}; i++ )); do
    thisfile=${argument_files[$i]}
	if [[ $thisfile =~ .*\.($sequence_exts) ]]; then
		has_sequences="y"
		inlist="n"
		tempdir=$(dirname "$thisfile")
		tempext=$(basename "$thisfile" | sed 's/.*\.\(.*\)/\1/')
		tempname=$(basename "$thisfile" | sed 's/\(.*\)\..*/\1/')
		collection=$(echo "$tempname" | sed 's/[0-9]*$//')
		numtemp=$(echo "$tempname" | grep -o -m 1 -e '[0-9]*$')
		
		if [[ "$numtemp" != [0-9]* ]]; then
			continue
		fi
		
		charcount=`printf "%02d" ${#numtemp}`
		new_path="${tempdir}/${collection}%${charcount}d.${tempext}"
		
		args=${#filelist[@]}
		if [[ $args != "0" ]]; then
			for (( e=1; e<=${args}; e++ )); do
				if [[ "${filelist[$e]}" == "$new_path" ]]; then
					inlist="y"
					break
				fi
			done
		fi
	
		if [[ "$inlist" == "n" ]]; then
			filelist+=( "$new_path" )
		fi
	elif [[ "$thisfile" =~ .*\.($mov_exts) ]]; then
		filelist+=( "$thisfile" )
	fi
done
unset argument_files

# Setup Platypus counter
count=0
args=${#filelist[@]}

if [[ "$args" == "0" ]]; then
	echo "No supported file types in batch list."
	exit 0
fi

# Ask for encoding settings
OLDIFS=$IFS
IFS=$'\n'
enc_sets=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output type." --height 150 --items $(for i in ${preset_name[@]}; do echo "$i"; done)`)
IFS=$OLDIFS
if [[ "${enc_sets[0]}" == "2" ]]; then
		echo "Encoding cancelled!"
		exit 1
fi

if [[ "$has_sequences" == "y" ]]; then
	select_fps=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "ffimgdrop" --text "Select output fps." --height 150 --items "${fps_options[@]}"`)
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

# Check for start approval and call the appropriate encoder
enc_type="${enc_sets[1]}"
for j in $(eval echo "{0..$(expr $preset_count - 1)}"); do
	if [[ "$enc_type" == $j ]]; then
		. presets/${preset_script[$j]}
		break
	fi
done

exit 0