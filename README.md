liTools
=======

Tools for compression and decompression of Little Inferno .pak files, and routines for managing mod creation and merging

Created by Daxar (aka MOM4Evr) from the infernofans forum -- http://infernofans.com/user/13

RUN ALL THESE PROGRAMS OFF A BACKUP COPY OF YOUR GAME, NOT THE ORIGINAL VERSION! You have been warned.

Also please note that it's best to have the the input files (Such as the Little Inferno executable, or the pak files you want to extract) in the same directory as these tools when running them. It'll save you a lot of hassle if you're using the command line, and it'll alleviate potential working directory issues if you're clicking and dragging files into executables.

Usage
=====

First, you'll need the three resource files "resource.pak", "embed.pak", and "frontend.pak" extracted from your game's executable. You can do this with 7zip or ResourceHacker (the files you want are in the subfolder 20480, resources 1, 2, and 3 respectively) if you so choose. liTools provides a tool to help out with this, however. Simply run:

strip.exe "Little Inferno.exe"

Or click and drag "Little Inferno.exe" into "strip.exe". This will pull all the .pak files out of your game's executable and strip the .pak files from your executable so it will read the .pak files from disk on the next run. Note that since this does modify your executable, BACK IT UP FIRST! See the "Utilities" section of this readme for programs to pull out the .pak files without modifying the game executable.

Once you have all three .pak files extracted, you can now decompress them into a useable form with liDecompress. You can decompress more than one file at a time if you wish. Simply drag the three .pak files into the executable, or run it like:

liDecompress.exe resource.pak embed.pak frontend.pak

The present version of the program attempts to reconstruct some of the resources from the extracted binary data, though something may go terribly horribly wrong. In this case, it should just spit out errors into the console and keep going. It should be fairly robust, but don't blame me if it crashes unexpectedly or does something bizarre.

Please note that can take a REALLY LONG TIME to decompress these files. resource.pak takes about 4 minutes for me. The console window should tell you the overall progress, and force-killing the program shouldn't have (m)any detrimental consequences, but be prepared to be patient. It does take a while. This program is fairly unoptimized, and it's a large file to process with a lot of compression/decompression steps for each resource.

Temporary files are written to temp/ and are destroyed if the program exits normally. Ignore them. The files you're after are in data/ and vdata/. The filenames are correct, but not all data formats are known yet, so not everything is openable. (For version 0.3, only png and ogg files are in the correct format)

The "residmap.dat" file is a modified version of the debug.pak file Allan Blomquist provided on the infernofans forum, in a stripped format that's easier for my program to work with. If you want to move liDecompress or liCompress around, be sure that this file also ends up in the same folder, or the program won't run. If you want, you can run debug.pak through the resource extractor as well (residmap.dat will end up in output/vdata/), but doing so isn't necessary.

To compress files again, type:

liCompress.exe resource.pak embed.pak frontend.pak

Please note that these .pak files might not exist beforehand (they don't have to). If they do exist, you can click-drag them into the executable as usual. For each pakfile, liCompress looks for a text file in the form [pakfile].pak.filelist.txt to know what files go inside that particular .pak file. You can create your own .pak files this way. liDecompress will autogenerate these .filelist.txt files when decompressing, so to save yourself a lot of typing, decompress first before compressing so all the .filelist.txt files are there. For now, this is how it works; this probably will change in a later version to become more user-friendly.

Mod creation and installation
=============================
Mods should be standard .pak files. To create your own mod, change the files you wish to change, and create a [modname].filelist.txt, which should be a list of all the filenames (relative to the Little Inferno executable) of all the files you have changed. For example, if you change the images data/items/Battery/Battery.png, data/animations/Intro/pchim1.png, data/animations/Intro/pchim2.png, and data/animations/Intro/pchim3.png, and you wish to name your mod "myMod", you'll have to create a file "myMod.filelist.txt" in the Little Inferno executable directory, and add the following lines to it:

data/items/Battery/Battery.png
data/animations/Intro/pchim1.png
data/animations/Intro/pchim2.png
data/animations/Intro/pchim3.png

Then, simply run 

liCompress myMod.pak

And it'll spit out the myMod.pak file. 
To install mods, run

modManage [pakfile1] [pakfile2] ... [pakfileN]

Where [pakfileX] is the mod you wish to install (or click-and-drag files into the executable). modManage will pull resource headers out of every .pak file to figure out what files should go in what .pak file, and will only modify the original .pak files as needed. If your mod changes files found in resource.pak, it may take a little while to install (about 30 seconds on my machine). Changes made later in the commandline will overwrite earlier changes; for example, if you run "modManage battery.pak battery2.pak", and both of these .pak files change data/items/Battery/Battery.png, the battery2.pak's version of the image will end up in the final game.

Changelog
=========
Version 0.1:
	- Initial release
	- Most .png and .png.normal images are correctly formatted

Version 0.2:
	- Added pullpakfiles and WinResource programs for pulling resources directly from the game's executable
	- Correct handling of greyscale images

Version 0.2.1:
	- Made strip program for extracting then stripping the .pak files
	- Made util/removeresc.exe and util/test.exe programs
	- Refactoring, minor changes to some tools
	
Version 0.3:
	- Added liCompress for compressing everything back into .pak files
	- For now, all files are compressed by default
	- Added util/repack.exe program for repacking .pak files into the game's exe

Version 0.3.1:
	- Multithreaded decompression/compression for speed
	- OGG streams are now left uncompressed

Version 0.3.2:
	- modManage program for merging in mods

Building (For my reference, makefile coming 'soon')
===================================================
Build with:
	g++ -Wall liDecompress.cpp threadDecompress.cpp zpipe.c ogg.cpp -O2 -o liDecompress.exe -lpng -lzlib -lttvfs -lvorbis -logg
	g++ -Wall liCompress.cpp threadCompress.cpp ogg.cpp zpipe.c -O2 -o liCompress.exe -lpng -lzlib -lttvfs -lvorbis -logg
	g++ -Wall strip.cpp -O2 -o strip.exe
	g++ -Wall modManage.cpp -O2 -o modManage.exe -lttvfs
	g++ -Wall repack.cpp -O2 -o repack.exe -lttvfs
	
Apologies for hastily-thrown-together code that isn't commented much at all.

Utilities ("util" subdirectory)
===============================

pullpakfiles.exe
----------------
If you'd rather just read the .pak files from your executable without stripping them out and modifying your game's executable, run:

util/pullpakfiles.exe "Little Inferno.exe"

Or copy "pullpakfiles.exe" from the "util" subdirectory into the same folder as "Little Inferno.exe" and click and drag "Little Inferno.exe" into it. Please note that this leaves the .pak files inside your executable, so the executable will not read the external pakfiles when run.


removeresc.exe
----------------
This program strips the .pak files out of your game's executable without saving them anywhere. DO NOT USE this program unless you have already run pullpakfiles to get the .pak files out, or unless you have the three .pak files in the directory already, since it just deletes the resources from the game without saving them anywhere. If you wish to run this program, use it in the same way as pullpakfiles:

util/removeresc.exe "Little Inferno.exe"

Or copy "removeresc.exe" from the "util" subdirectory into the same folder as "Little Inferno.exe" and click and drag "Little Inferno.exe" into it.


WinResource.exe
----------------
This program reads all the resources from inside an executable and spits out some data about them. In moments of dire need, it probably won't help you at all. So ignore it. Or run it with:

util/WinResource.exe "Little Inferno.exe"

Or copy "WinResource.exe" from the "util" subdirectory into the same folder as "Little Inferno.exe" and click and drag "Little Inferno.exe" into it. It'll spit out a file "resinfo.txt" containing basic information on the resources inside the Little Inferno executable. It may be useful, however, if you're unsure if your executable has been stripped or not (as if the file size alone wouldn't tell you). If resources with "Type: 20480" are all 4 bytes, you'll know it's stripped. Otherwise, probably not. At the very least, it's a fun program to play with.

test.exe
----------------
When run, spits out all the files found inside data/ and vdata/, recursively. As if you didn't know how to use "ls" or "dir" already.