ffdropenc Change Log
====================

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