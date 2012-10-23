ffdropenc
=========

An OSX droplet to batch encode video files for web upload. It is built off shell scripts, ffmpeg, x264, qtfaststart, cocoaDialog, and Platypus.


**Instructions**

There are two ways to launch the app.
 1) Drag your video files onto the droplet in Finder
 2) Double-click the droplet and drag your video files onto the window that opens
 
Batch lists are built from all files and folders drug together to the droplet icon/window. More files can be drug after the script has started, but this effectively creates a new batch. This new batch will not offer selection of Output Type or start encoding until the previous batch has completed.
 
**Other software**

ffmpeg - www.ffmpeg.org
x264 - http://www.videolan.org/developers/x264.html
ffmpeg-static (modified) - https://github.com/stvs/ffmpeg-static
qtfaststart - https://github.com/danielgtaylor/qtfaststart
cocoaDialog - http://mstratman.github.com/cocoadialog/
Platypus - http://sveinbjorn.org/platypus


**Build from source**

* Download and install Platypus from http://sveinbjorn.org/platypus
* Open ffdropenc.platypus
* Set variables: Script Path = webencoder.sh, Output = Progress Bar, Accept Dropped Items
* Add bundled resources: /bin, /presets, and exts.db
* Select app icon (I prefer HDD icon)
* Click Create 

**Known Issues**

* qtfaststart will not run on OSX 10.7 (Lion)
* ffmpeg needs to be recompiled with libvpx support


**Change Log**

v1.2
 * New: Presets for Vimeo 1080p HD, YouTube 1080p (Standard), and YouTube 1080p (Professional)
 * Fixed: Droplet asking for Output Type if launched without files

v1.1
 * Preliminary work for selecting encoding settings. Should be much easier to implement new encoding options in the future.
 * Added drop down settings selector via CocoaDialog.
 * Droplet now runs on OSX 10.6+

v1.0
 * Initial Release
 * Encodes for Vimeo HD.
 * Preliminary work for future support of YouTube HD.