ffdropenc Change Log
====================

##2014-02-15 - v1.8##

 * **NEW**: Global functions for common tasks. Right now these are just being used for progress tracking, but the potential exists for some powerful content filters (i.e. skipping files that already match the target output).  
 * **NEW**: Added ffprobe to internal bin. It's output is too much better than ffmpeg's. Hopefully no one minds the extra file size.  
 * **NEW**: A couple of deinterlacing presets. These currently use yadif. Hopefully a w3fdif-based preset will be coming in the future.  
 * **UPDATED**: All presets have been reorganized to make editing common values easier. This is a major overhaul of the whole preset system and I'm hoping it's going to be worth the effort.  
 * **UPDATED**: Little tweaks to most presets to make them more compatible with their targets.
 * **FIXED**: Scaling now works as expected. There was a major oversight with regards to scaling videos not 16x9. This should be fixed.  

##2014-01-27 - v1.7##

 * **NEW**: make-ffdropenc.sh - Automated build script for ffdropenc.app and distributable DMGs.  
 * **UPDATED**: Code reorganization to make further development easier.  
 * **UPDATED**: We no longer distribute ffmpeg or x264 binaries through GitHub. Use make-ffdropenc.sh or ffmpeg-static to compile your own.  
 * **UPDATED**: MPG/MPEG extensions added to file filters.  
 * **UPDATED**: Requisite bug fixes all the way around.
 * **NEW**: MKV (H.264/Audio Pass-thru) Preset. Useful for compressing DVDs for use by media servers.  

##2014-01-13 - v1.6.1##

 * **UPDATED**: All applicable presets now use CRF encoding. This should still produce comparable quality with a file-size reduction.  
 * **NEW**: Chromecast (1080p) preset.  
 * **UPDATED**: Most presets now use ffmpeg's internal faststart atom instead of qtfaststart.  
 * **UPDATED**: ProRes preset has experimental audio encoding.

##2013-12-21 - v1.6.0##

 * **NEW**: Custom Output Folder. ffdropenc now asks to select a custom output folder. If none is selected, 
 output files are saved next to the originals.  

##2013-12-17 - v1.5.2##
  
 * **NEW**: Chromecast (720p) preset based on Plex's transcoding profile.
 * **UPDATED**: Template presets now have all changes since v1.5.
 * **UPDATED**: Minor wording fix in Two-Pass Vimeo preset.

##2013-12-10 - v1.5.1 Changes##

 * **BUG FIX**: Fixed issue where anamorphic video would not scale down appropriately.
 * **UPDATED**: Added jpeg and ogg formats to approved extensions lists.
 * **BUG FIX**: Added wait time to progress tracker to hopefully combat random parse errors.

##2013-10-07 - v1.5 Features and Changes##

After branching ffdropenc off to make ffimgdrop, I felt weird about having a second program just 
for a different input type. After a short (yet informative) lifespan, ffimgdrop is no more. I've 
merged it back into ffdropenc, bringing image sequence inputs to all of ffdropenc's preset options. 
I've also brought in a few small improvements to the  whole experience.  
  
 * **NEW Image Sequence Inputs**: Sequentially numbered JPGs and PNGs can now be passed for transcoding. 
 This will work with all presets. Numbering must be at the end of the filename, the count must start at 1, 
 and the numbering must include leading zeros. E.g. "sequence0001.jpg, sequence0002.jpg, … sequence9999.jpg". 
 The theoretical limit of a sequence is a 99 digit numeral that consists of all 9's. 
 _NOTE: You can also drop folders of images onto ffdropenc. If there are multiple sequences detected inside a 
 single folder, ffdropenc will encode each one separately. However, ffdropenc checks every single file passed to it, 
 as well as every single file in every single folder passed to it, to find sequences. In practice, it's 
 considerably faster to just drop a single image from each sequence you wish to encode._ 
 * **IMPROVED Scaling**: Most presets will never scale higher than the input video, but they will scale down. 
 The exception to this is the BD preset. It was built for 1080p I/O.
 * **UPDATED Encoder**: ffmpeg updated to v2.0.
 * **UPDATED Installer**: Easy drag-and-drop installer dmg.
 * **UPDATED Graphics**: These are still a work in progress, but ffdropenc now sports a new logo and graphical 
 style. Expect to see this carry over into v2.0.
