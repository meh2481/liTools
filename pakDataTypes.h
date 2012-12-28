//What we were given from Allan

#ifndef LIDECOMPRESS_H
#define LIDECOMPRESS_H

#include <cstdio>
#include <tinyxml2.h>
#include <stdint.h>
#include <string>
#include <list>
#include <VFS.h>
#include <VFSTools.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
using namespace tinyxml2;
using namespace std;
#define i32 int32_t
#define u32 uint32_t
#define f32	float
#define MAX_NUM_THREADS 128

//Mark I structures - .pak file headers
typedef struct 
{
	i32 pakVersion;
	i32 numItems;
} blobHeader;

typedef struct
{
	u32 id;
	u32 flags;
	u32 offset;
	u32 size;
} resourceHeader;

typedef struct
{
	i32 compressedSizeBytes;
	i32 uncompressedSizeBytes;
} compressedHeader;

#define FLAG_NOCOMPRESSION	0x00
#define FLAG_ZLIBCOMPRESSED	0x01

//Mark II structures - PNG headers and residmap.dat headers
typedef struct
{
	i32 count;
	i32 offset;
} BinHdrPtr;

typedef struct
{
	BinHdrPtr maps;
	BinHdrPtr stringTableBytes;
} ResidMapHeader;

typedef struct
{
	u32 resId;
	i32 strId;
} MappingHeader;

typedef struct
{
	i32 numStrings;
	i32 numPointers;
} StringTableHeader;

typedef struct
{
	i32 pointerIndex;
	i32 pointerCount;
} StringTableEntry;

typedef struct
{
	u32 languageId;
	i32 offset;
} StringPointerEntry;

#define LANGID_ENGLISH  0x656E
#define GREYSCALE_PNG	0x08
#define STANDARD_PNG	0x01

typedef struct
{
	u32 width;
	u32 height;
	u32 flags;
} ImageHeader;

//Mark III structures - wordPackDict.dat headers
typedef struct
{
	BinHdrPtr words;
	BinHdrPtr stringTableBytes;
} wordPackDictHeader;

typedef struct
{
	i32 wordStrId;
	f32 probability;
} wordHeader;

//Structures for my use
typedef struct
{
	compressedHeader cH;
	u32 id;
	bool bCompressed;
} pakHelper;

typedef struct
{
	string sIn;
	string sFilename;
	bool bCompressed;
} ThreadConvertHelper;

//Mark IV structures - sndmanifest.dat structures
typedef struct
{
	BinHdrPtr sounds;
	BinHdrPtr takes;
} soundManifestHeader;

typedef struct
{
	u32 logicalId;
	i32 firstTakeIdx;
	i32 numTakes;
} soundTakeGroup;

typedef struct
{
	u32 resId;
	i32 channels;
	i32 samplesPerSec;
	i32 sampleCountPerChannel;
	i32 vorbisWorkingSetSizeBytes;
	i32 vorbisMarkersSizeBytes;
	i32 vorbisPacketsSizeBytes;
} takeRecord;

//Mark V structures - itemmanifest.dat structures
typedef struct
{
	BinHdrPtr itemsManifest;
	BinHdrPtr normalDeps;
	BinHdrPtr soundDeps;
	BinHdrPtr effectDeps;
	BinHdrPtr itemDeps;
	BinHdrPtr itemsBinDataBytes;
} itemManifestHeader;

typedef struct
{
	u32 itemId;
	u32 animResId;
	i32 recentlyModifiedRank;
	i32 firstNormalDepends;
	i32 numNormalDepends;
	i32 firstSoundDepends;
	i32 numSoundDepends;
	i32 firstEffectDepends;
	i32 numEffectDepends;
	i32 firstItemDepends;
	i32 numItemDepends;
	u32 catalogIconColorItemTexResId;
	u32 catalogIconColorBGTexResId;
	u32 catalogIconGreyBGTexResId;
	i32 binDataOffsetBytes;
} itemManifestRecord;

typedef struct
{
	u32 normalTexResId;
} normalDependency;

typedef struct
{
	u32 soundResId;
} soundDependency;

typedef struct
{
	u32 effectResId;
} effectDependency;

typedef struct
{
	u32 itemResId;
} itemDependency;


//global functions
int compdecomp(const char* cIn, const char* cOut, int iCompress = false);	//Compress/decompress a file using zlib
int binaryToOgg( const char* in, const char* out );			//Function from Allan to convert a game sound file to .ogg
int oggToBinary( const char* in, const char* out );			//Function from Allan to convert an .ogg file to the game's sound format
takeRecord getOggData( const char* cFile );					//Grab the data from an OGG file to populate sndManifest.dat
void threadedDecompress();									//Start threaded decompression
void threadedCompress();									//Start threaded compression
bool convertToPNG(const char* cFilename);					//Convert a game image file to PNG
bool convertFromPNG(const char* cFilename);					//Convert a PNG image to a game image file
bool wordPackToXML(const char* cFilename);					//Convert wordPackDict.dat to XML
bool XMLToWordPack(const char* cFilename);					//Convert wordPackDict.dat.xml back to binary .dat form
bool sndManifestToXML(const char* cFilename);				//Convert sndManifest.dat to XML
bool XMLToSndManifest(const char* cFilename);				//Convert sndManifest.dat.xml back to binary .dat form
void initSoundManifest();									//Read in sndManifest.dat so that we can have the correct filenames for all sounds
u32 getSoundId(string sSound);								//Get a sound ID from the filename
string getSoundName(u32 soundResId);						//Get a sound filename from the sound ID
bool itemManifestToXML(const char* cFilename);				//Convert itemmanifest.dat to XML
bool XMLToItemManifest(const char* cFilename);				//Convert itemmanifest.dat.xml back to binary .dat form
void initResMap();											//Read in residmap.dat so that we can have the correct filenames for all resource files
bool residMapToXML(const char* cFilename);					//Convert residmap.dat to XML
bool XMLToResidMap(const char* cFilename);					//Convert residmap.dat.xml back to binary .dat form
const char* getName(u32 resId);								//Get a resource filename from the resource ID
u32 getResID(string sName);									//Get a resource ID from its filename


#endif