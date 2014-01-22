#!/bin/sh
# Auto-build ffdropenc.app and DMG

set -e

echo
echo "----Auto-build ffdropenc.app----"

# Check that we're running from the right folder
if [[ "$PWD" != *ffdropenc ]]; then
	echo "ERROR: Not inside the ffdropenc root folder."
	echo "Please cd to the ffdropenc folder."
	echo
	exit 1
fi

# Get options
usage() { echo "Usage: $0 [-f] [-c] [-d]" 1>&2; echo; exit 1; }
free=""
custom_encoder=""
dmg=""
builddate=$(date +"%Y%m%d")

while getopts "fcd" o; do
    case "${o}" in
        f)
            free=1
            ;;
        c)
            custom_encoder=1
            ;;
        d)
        	dmg=1
        	;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

# Check for prerequisite software
echo "Checking environment..."
ff_root="$PWD"
if command -v platypus >/dev/null; then
	echo "Platypus found..."
else
	echo "ERROR: Platypus command line utility not found. Please install before continuing."
	echo
	exit 1
fi

if [[ $custom_encoder == "" ]]; then
	if pkgutil --pkg-info=com.apple.pkg.CLTools_Executables >/dev/null; then
		echo "Xcode Command Line Tools found..."
		if [[ $free == "1" ]]; then
			status="free"
		else 
			status="nonfree"
		fi
	else
		echo "ERROR: Xcode Command Line Tools not found. Please install before continuing."
		echo
		exit 1
	fi
else
	echo "Using custom encoder binaries. Xcode Command Line Tools not required."
	status="custom"
fi

if [[ $custom_encoder == "1" ]]; then
	if [[ ! -e "$ff_root"/source/bin/ffmpeg ]] || [[ ! -e "$ff_root"/source/bin/x264 ]]; then
		echo "ERROR: Custom binary option [-c] used but custom encoder binaries not found in '$ff_root/source/bin'."
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

# Copy latest ffdropenc source files and get version
echo "Copying latest source files..."
echo
cp -rf "$ff_root"/source ./ffdropenc
cp "$ff_root"/extras/graphics/ffdropenc.icns ./
version=$(sed -n /Version/p "ffdropenc/ffdropenc.sh" | sed 's/# Version //')

if [[ $custom_encoder == "" ]]; then
	# Download ffmpeg-static...
	echo "Building new encoder binaries..."
	echo "Downloading ffmpeg-static..."
	curl -s -L https://github.com/csparker247/ffmpeg-static/tarball/master | tar zx
	mv *ffmpeg-static*/ ffmpeg-static/

	# Make ffmpeg-static
	echo "Building ffmpeg-static..."
	cd ffmpeg-static
	if [[ $free == "1" ]]; then
		./build.sh -f
	else
		./build.sh
	fi
	cd ..
	cp ffmpeg-static/target/bin/ffmpeg ffdropenc/bin/ffmpeg
	cp ffmpeg-static/target/bin/x264 ffdropenc/bin/x264
	echo
	echo ffmpeg-static built.
fi

# Make ffdropenc.app
echo "Making ffdropenc-${version}.app..."

platypus -D -a ffdropenc -i ffdropenc.icns -V "$version" -u "Seth Parker" \
-f "ffdropenc/bin" -f "ffdropenc/presets" -f "ffdropenc/resources" \
-I "com.ffdropenc.ffdropenc" "ffdropenc/ffdropenc.sh" ffdropenc-${version}-${status}.app

# Cleanup
rm -rf ffdropenc/
rm ffdropenc.icns

echo
echo ffdropenc-${version}-${status}.app built successfully!

if [[ $dmg == "1" ]]; then
	echo
	echo Making ffdropenc-${version}-${status}-${builddate}.dmg...
	curl -s -L https://github.com/csparker247/yoursway-create-dmg/tarball/master | tar zx
	mv *create-dmg*/ create-dmg/
	mkdir dmg
	cp -rf ffdropenc-${version}-${status}.app dmg/ffdropenc.app
	cp "$ff_root"/extras/graphics/ffdropenc_volume.icns ffdropenc_volume.icns
	cp "$ff_root"/extras/graphics/png/installer_background.png installer_background.png
	create-dmg/create-dmg \
		--volname ffdropenc-${version}-${status}-${builddate} \
		--volicon ffdropenc_volume.icns \
		--background installer_background.png \
		--window-pos 200 120 \
		--window-size 800 400 \
		--icon-size 100 \
		--icon ffdropenc.app 200 190 \
		--hide-extension ffdropenc.app \
		--app-drop-link 600 185 \
		ffdropenc-${version}-${status}-${builddate}.dmg \
		dmg/
	rm -rf dmg/
	rm -rf create-dmg/
	rm installer_background.png
	rm ffdropenc_volume.icns
fi

exit 0
