#!/bin/sh
# Auto-build ffdropenc.app and DMG

set -e
set -u

echo
echo "-------------------------"
echo "| ffdropenc.app Creator |"
echo "-------------------------"
echo

# Check that we're running from the right folder
if [[ "$PWD" != *ffdropenc ]]; then
	echo "ERROR: Not inside the ffdropenc root folder."
	echo "Please cd to the ffdropenc folder."
	echo
	exit 1
fi

# Get options
usage() { 
cat << EOF
Usage: $0 [options]

This script is used to automatically create ffdropenc.app and distributable DMGs.

OPTIONS:
-h		Show this message.
	
-b type		Selects which ffmpeg and x264 binaries to use: 
			nonfree  	Build new binaries using non-free libfdk-aac.
					Default value, except when using sym-link (-s) option.
					You can make new binaries for use with ffdevenc.app by running:
					'make-ffdropenc.sh -b nonfree -s'
						
			free		Build new binaries using free codecs only.
						
			custom		Use ffmpeg and x264 binaries found in "../ffdropenc/source/bin".
					Default value when using sym-link (-s) option.
			
-d		Generate Installer DMG.

-s		Create sym-linked development application, ffdevenc.app, using files in "ffdropenc/source".
  		Disables DMG (-d) output. Defaults to custom binaries. Currently not usable with free encoder option.
		
	
	
EOF
exit 1
}

ff_root="$PWD"
version=$(sed -n /Version/p "source/ffdropenc.sh" | sed 's/# Version //')
builddate=$(date +"%Y%m%d")
default=1
status="nonfree"
dmg=0
dev=0

while getopts ":hb:ds" arg; do
    case "${arg}" in
        h)
        	usage
        	;;
        b)
            status="$OPTARG"
            case "$status" in
				nonfree | free | custom) ;;
				*) echo ERROR: Unrecognized value for option -b: "$status"
				echo
				usage ;;
			esac
            default=0
            ;;
        d)
        	dmg=1
        	;;
        s)
        	dev=1
        	;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [[ $dev == 1 ]]; then
	if [[ $dmg == 1 ]]; then
		echo "WARNING: DMG output is not currently supported in sym-link mode! Disabling DMG output."
		echo
		dmg=0
		sleep 1
	fi
	if [[ $status == "free" ]]; then
		echo "WARNING: Free binaries are not currently supported in sym-link mode! Defaulting to custom encoders."
		echo
		default=1
		sleep 1
	fi
	if [[ $default == 1 ]]; then
		status="custom"
	fi
else
	if [[ $default == 1 ]]; then
		status="nonfree"
	fi
fi

# Check for prerequisite software
echo "Checking environment..."
if command -v platypus >/dev/null; then
	echo "Platypus found..."
else
	echo "ERROR: Platypus command line utility not found. Please install before continuing."
	echo
	exit 1
fi

if [[ $status == "nonfree" || $status == "free" ]]; then
	if command -v gcc >/dev/null; then
		echo "gcc compiler found..."
	elif command -v clang >/dev/null; then
		echo "clang compiler found..."
	else
		echo "ERROR: Compatible C compiler not found. Please install before continuing."
		echo
		exit 1
	fi
else
	echo "Using custom encoder binaries. Xcode Command Line Tools not required."
fi

if [[ $status == "custom" ]]; then
	if [[ ! -e "$ff_root"/source/bin/ffmpeg ]] || [[ ! -e "$ff_root"/source/bin/x264 ]]; then
		echo "ERROR: Custom binary option used but custom encoder binaries not found in '$ff_root/source/bin'."
		echo
		exit 1
	fi
	echo "Custom encoder binaries found..."
fi


# Create build folder
echo
echo "Setting up build folder..."
if [[ -d build ]]; then
	rm -rf build/
	mkdir build
else
	mkdir build
fi
cd build

# Build new encoders
if [[ $status == "nonfree" || $status == "free" ]]; then
	# Download ffmpeg-static...
	echo "Building new encoder binaries..."
	echo "Downloading ffmpeg-static..."
	curl -s -L https://github.com/csparker247/ffmpeg-static/tarball/master | tar zx
	mv *ffmpeg-static*/ ffmpeg-static/

	# Make ffmpeg-static
	echo "Building ffmpeg-static..."
	cd ffmpeg-static
	if [[ $status == "free" ]]; then
		./build.sh -f
	elif [[ $status == "nonfree" ]]; then
		./build.sh
	fi
	echo
	echo "New encoder binaries built..."
fi

cd "$ff_root"/build

# Copy latest ffdropenc source files and get version number
echo "Preparing source files..."
echo
if [[ "$dev" == "0" ]]; then
	cp -rf "$ff_root"/source ./appsource
	buildsrc="$ff_root/build/appsource"
	buildname="ffdropenc"

	if [[ "$status" == "nonfree" || "$status" == "free" ]]; then 
		cp ffmpeg-static/target/bin/ffmpeg "$buildsrc"/bin/ffmpeg
		cp ffmpeg-static/target/bin/x264 "$buildsrc"/bin/x264
		rm -rf ffmpeg-static/
	fi
	
	# Patch presets to use built-in aac when using free binaries
	if [[ "$status" == "free" ]]; then
		echo "Patching presets for use with free codecs..."
		grep -rl "\-c:a libfdk_aac" "$buildsrc"/presets | xargs sed -i "" 's/-c:a libfdk_aac/\-strict experimental \-c:a aac/g'
	fi
	
elif [[ "$dev" == "1" ]]; then
	buildsrc="$ff_root/source"
	buildname="ffdevenc"
	
	if [[ "$status" == "nonfree" ]]; then
		cp ffmpeg-static/target/bin/ffmpeg "$buildsrc"/bin/ffmpeg
		cp ffmpeg-static/target/bin/x264 "$buildsrc"/bin/x264
		rm -rf ffmpeg-static/
	fi

fi

# Make OSX app...
echo "Making ${buildname}.app..."

platypus -D -a "$buildname" -i "$ff_root/extras/graphics/$buildname.icns" -V "$version" -u "Seth Parker" \
-f "$buildsrc/bin" -f "$buildsrc/presets" -f "$buildsrc/resources" \
-I "com.ffdropenc.$buildname" $(if [[ "$dev" == 1 ]]; then echo "-d"; fi) \
"$buildsrc/ffdropenc.sh" "${buildname}.app"

echo
echo "${buildname}.app" built successfully!

if [[ $dmg == "1" ]]; then
	echo
	echo Making ${buildname}-${version}-${status}-${builddate}.dmg...
	curl -s -L https://github.com/csparker247/yoursway-create-dmg/tarball/master | tar zx
	mv *create-dmg*/ create-dmg/
	mkdir dmg
	cp -rf ${buildname}.app dmg/${buildname}.app
	create-dmg/create-dmg \
		--volname ${buildname}-${version}-${status}-${builddate} \
		--volicon "$ff_root/extras/graphics/ffdropenc_volume.icns" \
		--background "$ff_root/extras/graphics/installer_background.png" \
		--window-pos 200 120 \
		--window-size 800 400 \
		--icon-size 100 \
		--icon ${buildname}.app 200 190 \
		--hide-extension ${buildname}.app \
		--app-drop-link 600 185 \
		${buildname}-${version}-${status}-${builddate}.dmg \
		dmg/
	rm -rf dmg/
	rm -rf create-dmg/
fi
echo
echo All files built. Exiting.
echo
exit 0
