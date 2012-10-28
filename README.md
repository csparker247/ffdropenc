ffdropenc
=========

An OSX droplet to batch encode video files for web upload. It is built off shell scripts, ffmpeg, x264, qtfaststart, cocoaDialog, and Platypus.

**Latest updates**

* 10.28.2012 - Fixed qtfaststart issue. Fixed scaling issues. Encoding options build from presets folder, making implementation of new encoding options a lot easier. Added support for Bluray 1080p streams (via x264).

**Known Issues**

* ffmpeg needs to be recompiled with libvpx support

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