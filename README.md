ffdropenc
=========
An OSX droplet to batch encode video files.

* Doesn't work with files without extensions (which you can sometimes run into on OSX). Make sure your exported files include one of the approved extensions in the database files.
* Some presets use H.264 video profiles that are unsupported by older versions of Quicktime player. This will usually appear as grey or "corrupted" output during playback. Test your file in VLC to ensure proper encoding.  
* The ProRes422 codec doesn't like input files without audio tracks. We're currently seeking a workaround.

**Instructions**

There are two ways to launch the app.
 1) Drag your video files onto the droplet in Finder  
 2) Double-click the droplet and drag your video files onto the
 window that opens
 
Batch lists are built from all files and folders drug together to the
droplet icon/window. More files can be drug after the script has
started, but this effectively creates a new batch. This new batch will
not offer selection of Output Type or start encoding until the previous
batch has completed.

**Download**
  
Due to licensing concerns, I can no longer distribute binaries with fdk-aac. However, following the **Build from source** instructions below will compile 
ffdropenc with a fdk-aac-enabled ffmpeg. This will give you much better sound quality and is the preferred installation method.  
  
The freely distributable OSX app can be downloaded from [here](https://dl.dropboxusercontent.com/u/13015285/ffdropenc-latest-free.dmg "Download ffdropenc on Dropbox").  

**Build from source**
  
* Install Xcode and the Xcode command line tools.  
* Install pkg-config (via Homebrew)
* Download and install [Platypus](http://sveinbjorn.org/platypus).
* Install the [Platypus command line tools](http://sveinbjorn.org/files/manpages/PlatypusDocumentation.html#51).  
* Run the following commands:  

	> $ git clone https://github.com/csparker247/ffdropenc.git  
	> $ cd ffdropenc/  
	> $ ./make-ffdropenc.sh 

To see other build options, view the help documentation:  
> $ ./make-ffdropenc.sh -h  
  
**Test Videos**  

Test videos are no longer kept in the repository for space concerns. Users with After Effects can render the files themselves using test-video.aep. 
You can also download MP4 (H.264, AAC) versions of the files from [here](https://dl.dropboxusercontent.com/u/13015285/ffdropenc-testvideos.zip).
 
**Other software**

ffmpeg - www.ffmpeg.org  
x264 - http://www.videolan.org/developers/x264.html  
ffmpeg-static (modified) - https://github.com/csparker247/ffmpeg-static  
yoursway-create-dmg - https://github.com/csparker247/yoursway-create-dmg  
cocoaDialog - http://mstratman.github.com/cocoadialog/  
Platypus - http://sveinbjorn.org/platypus  
