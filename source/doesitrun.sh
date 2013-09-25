#!/bin/bash
PATH=./bin:$PATH

# doesitrun.sh
# Tests for ffdropenc functionality
echo
echo "-----------------------"
echo "ffdropenc: Does It Run?"
echo "-----------------------"
echo


# Program setup

# Build encoding options list
preset_script=()
preset_name=()
preset_count="0"

echo "Counting Presets..."
echo
for i in presets/*.sh; do
	preset_type=$(sed -n /optname/p "$i" | sed 's/# optname //')
	if [[ "$preset_type" != "" ]]; then
		preset_script+=( "$(basename "$i")" )
		preset_name+=( "$preset_type" )
	fi
done
# Count number of presets
preset_count="${#preset_name[@]}"
echo "Number of Presets Found: $preset_count"
echo "Presets are:"
printf '\t%s\n' "${preset_name[@]}"
echo
echo

# FPS Setup
fps_options=( "24" "25" "29.97" "30" "60" )
fps_count="${#fps_options[@]}"

# Collect list of approved file extensions
echo "Reading extension databases..."
echo
filters=`cat filters.db`
sequence_exts=`cat seqexts.db`
mov_exts=`cat movexts.db`
if [[ $filters == "" ]]; then
	echo "ERROR: Cannot read filters.db."
else
	echo "Accepted extensions are:"
	printf '\t%s\n' "$filters"
fi
if [[ $sequence_exts == "" ]]; then
	echo "ERROR: Cannot read seqexts.db."
else
	echo "Accepted image sequence extensions are:"
	printf '\t%s\n' "$sequence_exts"
fi
if [[ $mov_exts == "" ]]; then
	echo "ERROR: Cannot read movexts.db."
else
	echo "Accepted movie file extensions are:"
	printf '\t%s\n' "$mov_exts"
fi
echo
echo

# Setup qtfaststart
echo "Looking for qtfaststart..."
if [[ -e bin/qtfaststart/qtfaststart ]]; then
	echo "Found qtfaststart."
	qtfaststart="bin/qtfaststart/qtfaststart"
else
	echo "ERROR: Cannot find qtfaststart in expected location /bin/qtfaststart. Does it exist?"
fi
echo
echo

# Manually set argument for doesitrun.sh
if [[ -e "../test_video/ffdropenc-RandomShapes_10secs.mp4" ]]; then
	set -- "../test_video/ffdropenc-RandomShapes_10secs.mp4"
else
	echo "ERROR: Test video missing from source folder. Does it exist?"
fi

# Build list of files to encode, using only files from approved extensions list.
# Convert arguments to array, filtering out unsupported files
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

echo "File list is:"
printf '\t%s\n' "${filelist[@]}"
echo

# Setup Platypus counter
count=0
args=${#filelist[@]}

if [[ "$args" == "0" ]]; then
	echo "No supported file types in batch list."
	exit 0
fi

echo "Testing cocoaDialog..."
if [[ -e bin/cocoaDialog.app/Contents/MacOS/cocoaDialog ]]; then
	echo "Found cocoaDialog."
else
	echo "ERROR: cocoaDialog not found."
fi
# Ask for encoding settings
echo
echo "Getting encoding settings via cocoaDialog..."

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
		echo
		echo "ffdropenc calling for presets/${preset_script[$j]}"
		if [[ -e presets/${preset_script[$j]} ]]; then
			echo "Preset exists in folder."
		else
			echo "ERROR: Preset does not exist!"
		fi
		break
	else
		echo "$enc_type != $j: Nothing to be done."
	fi
done
echo
echo "Testing Complete!"
echo

exit 0