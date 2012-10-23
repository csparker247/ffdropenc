ffdropenc
=========

An OSX droplet to batch encode video files for web upload. It is built off shell scripts, ffmpeg, x264, cocoaDialog, and Platypus.


**Instructions**

There are two ways to launch the app.

 1) Drag your video files onto the droplet in Finder.

 or

 2) Double-click the droplet and drag your video files onto the window that opens.



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