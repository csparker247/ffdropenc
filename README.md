<img src="http://www.csethparker.com/imgs/xtras/banner2.jpg" width="100%" />
=========
An OSX droplet to batch encode video files.

**Latest updates**

* 02.15.2014 - Update to v1.8. Introducing global functions and deinterlacing presets. Major overhaul to preset organization. Ability to analyze files via ffprobe. Improved scaling.  
* 01.27.2014 - Update to v1.7. Created ffdropenc auto-build system. Code reorg. Various preset cleanups and bug fixes.  
* 01.13.2014 - Presets use CRF encoding. Experimental ProRes audio. Now uses ffmpeg faststart atom.  
* 12.21.2013 - Support for custom output folder.
* 12.17.2013 - New Chromecast preset. Templates updated. Minor wording fix.
* 12.10.2013 - Added new formats to approved extensions list. Fixed scaling issues with anamorphic video.
* 09.25.2013 - Update to ffdropenc v1.5. Adds smarter scaling and image sequence inputs. 
* 05.02.2013 - Major update to progress tracking. The progress bar should now display batch progress in realtime for presets. Added template presets to aid in creation of new presets. 
* 11.2012-03.2013 - Lots of small changes. Check the commits for more details.
* 11.20.2012 - Added AC3 audio output to Bluray preset. Added two-pass version of Vimeo 720p preset.
* 11.7.2012 - Synced to current snapshots of ffmpeg and x264. Replaced faac with fdk-aac. Presets utilizing aac encoding updated accordingly (-c:a libfdk_aac). Libvpx support added.
* 10.28.2012 - Fixed qtfaststart issue. Fixed scaling issues. Encoding options build from presets folder, making implementation of new encoding options a lot easier. Added support for Bluray 1080p streams (via x264).

**Known Issues/Special Notes**

* Doesn't work with files without extensions (which you can sometimes run into on OSX). Make sure your exported files include one of the approved extensions in the database files.
* Some presets use H.264 video profiles that are unsupported by older versions of Quicktime player. This will usually appear as grey or "corrupted" output during playback. Test your file in VLC to ensure proper encoding.

**Instructions**

There are two ways to launch the app.  
 1) Drag your video files onto the droplet in Finder  
 2) Double-click the droplet and drag your video files onto the window that opens  
 
Batch lists are built from all files and folders drug together to the droplet icon/window. More files can be drug after the script has started, but this effectively creates a new batch. This new batch will not offer selection of Output Type or start encoding until the previous batch has completed.

**Download**
  
Due to licensing concerns, I can no longer distribute binaries with libfdk_aac. However, following the **Build from source** instructions below will compile 
ffdropenc with a libfdk\_aac-enabled ffmpeg. This will give you much better sound quality and is the preferred installation method.  
  
The latest OSX app can be downloaded from [here](https://dl.dropboxusercontent.com/u/13015285/ffdropenc-latest-free.dmg "Download ffdropenc on Dropbox").  

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
 
**Other software**

ffmpeg - www.ffmpeg.org  
x264 - http://www.videolan.org/developers/x264.html  
ffmpeg-static (modified) - https://github.com/csparker247/ffmpeg-static  
yoursway-create-dmg - https://github.com/csparker247/yoursway-create-dmg  
cocoaDialog - http://mstratman.github.com/cocoadialog/  
Platypus - http://sveinbjorn.org/platypus  



