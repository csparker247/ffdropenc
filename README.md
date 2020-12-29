![ffdropenc](graphics/png/banner.png)

[![CI](https://github.com/csparker247/ffdropenc/workflows/Build/badge.svg?branch=develop)](https://github.com/csparker247/ffdropenc/actions)

**ffdropenc** is an easy-to-use macOS droplet application for batch encoding video files. Drag your videos (or image 
sequences) to the app window or icon, select a preset and an output location, and click OK. It's that easy!

## Installation
The latest DMG installer can be downloaded from the [Releases page](https://github.com/csparker247/ffdropenc/releases).
Double-click the DMG and drag the app icon to the Applications folder. **ffdropenc** isn't code signed 
[yet](https://github.com/csparker247/ffdropenc/issues/28), so you'll need to right-click the app icon and select
**Open** on first launch.

## Custom presets
We don't officially support custom preset files, but it is a planned feature. In the meantime, feel free to open a Merge
Request adding your custom preset to the [built-in preset repository](apps/presets).

## Custom FFmpeg
**ffdropenc** comes bundled with the latest release version of the 
[evermeet FFmpeg binary](https://evermeet.cx/ffmpeg/), however, you may want to use your own binary for various reasons.
That's alright by us! In `ffdropenc -> Preferences`, you'll find an option for selecting the FFmpeg binary of your 
choice!

## Cross-platform support
This project was written on macOS but was designed with cross-platform compatibility in mind. All code is written using 
C++17 with no non-standard extensions, and it uses Qt5 as the GUI framework. The only reason it's not already a 
cross-platform release is because I haven't had the time to figure out the details for Windows and Linux systems. If 
you're interested in getting releases for those platforms up and running, see the 
[Windows](https://github.com/csparker247/ffdropenc/issues/29) and 
[Linux](https://github.com/csparker247/ffdropenc/issues/30) release issues in the Issue Tracker.

## Build from source
### Requirements