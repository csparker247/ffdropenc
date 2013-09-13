<img src="http://www.csethparker.com/imgs/xtras/banner2.jpg" width="100%" />
=========
An OSX droplet to batch encode video files for web upload. It is built off shell scripts, ffmpeg, x264, qtfaststart, cocoaDialog, and Platypus.

**Latest updates**

* 05.02.2013 - Major update to progress tracking. The progress bar should now display batch progress in realtime for presets. Added template presets to aid in creation of new presets. 
* 11.2012-03.2013 - Lots of small changes. Check the commits for more details.
* 11.20.2012 - Added AC3 audio output to Bluray preset. Added two-pass version of Vimeo 720p preset.
* 11.7.2012 - Synced to current snapshots of ffmpeg and x264. Replaced faac with fdk-aac. Presets utilizing aac encoding updated accordingly (-c:a libfdk_aac). Libvpx support added.
* 10.28.2012 - Fixed qtfaststart issue. Fixed scaling issues. Encoding options build from presets folder, making implementation of new encoding options a lot easier. Added support for Bluray 1080p streams (via x264).

**Known Issues/Special Notes**

* Script doesn't work with files without extensions (which you can sometimes run into on OSX). Make sure your exported files include one of the approved extensions in the exts.db file.
* Some presets use H.264 video profiles that are unsupported by certain versions of Quicktime player. This will usually appear as grey or "corrupted" output during playback. Test your file in VLC to ensure proper encoding.

**Instructions**

There are two ways to launch the app.  
 1) Drag your video files onto the droplet in Finder  
 2) Double-click the droplet and drag your video files onto the window that opens  
 
Batch lists are built from all files and folders drug together to the droplet icon/window. More files can be drug after the script has started, but this effectively creates a new batch. This new batch will not offer selection of Output Type or start encoding until the previous batch has completed.
 
**Other software**

ffmpeg - www.ffmpeg.org  
x264 - http://www.videolan.org/developers/x264.html  
ffmpeg-static (modified) - https://github.com/csparker247/ffmpeg-static  
qtfaststart - https://github.com/danielgtaylor/qtfaststart  
cocoaDialog - http://mstratman.github.com/cocoadialog/  
Platypus - http://sveinbjorn.org/platypus  


**Build from source**

* Download and install Platypus from http://sveinbjorn.org/platypus
* Open ffdropenc.platypus
* Set variables: Script Path = ffdropenc.sh, Output = Progress Bar, Accept Dropped Items
* Add bundled resources: /bin, /presets, and exts.db
* Select app icon (I prefer HDD icon)
* Click Create 
