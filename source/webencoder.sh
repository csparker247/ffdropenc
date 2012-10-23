#!/bin/sh
PATH=$PATH:./bin

# Web Encoder - ffmpeg - v1.2
# This is a droplet to encode videos for upload to YouTube and Vimeo.
# This droplet is based off the Bluray Encoding Droplet. It uses ffmpeg as its transcoder.

# Quit if no files/arguments given
if [ $# -eq 0 ]; then
	exit 0
fi

# Build list of files to encode, using only files from approved extensions list.
filelist=()
exts=`cat exts.db` 
# echo $exts

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
enc_sets=(`bin/cocoaDialog.app/Contents/MacOS/cocoaDialog standard-dropdown --title "Web Encoder Droplet 1.1" --text "Select output type." --height 150 --items "Vimeo 720p HD" "Vimeo 1080p HD" "YouTube 1080p HD (Standard)" "YouTube 1080p HD (Professional)"`)
	
# Check for start approval and call the appropriate encoder
if [ "${enc_sets[0]}" = "0" ]; then
		echo "Encoding cancelled!"
		exit 1
	else
		echo PROGRESS:5
		
		# Vimeo 720p Condition
		while [ "${enc_sets[1]}" = "0" ]; do
			. presets/vimeo720.sh
			enc_sets[1]=""
		done
		
		# Vimeo 1080 Condition
		while [ "${enc_sets[1]}" = "1" ]; do
			. presets/vimeo1080.sh		
			enc_sets[1]=""
		done
		
		# YouTube Standard 1080 Condition
		while [ "${enc_sets[1]}" = "2" ]; do
			. presets/yt1080.sh		
			enc_sets[1]=""
		done
		
		# YouTube Pro 1080 Condition
		while [ "${enc_sets[1]}" = "3" ]; do
			. presets/ytpro1080.sh		
			enc_sets[1]=""
		done
fi	

exit 0