//What we were given from Allan

#ifndef LIDECOMPRESS_H
#define LIDECOMPRESS_H

#include <cstdio>
#include <tinyxml2.h>
#include <stdint.h>
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
#define UNICODE
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

typedef struct
{
	f32 x;
	f32 y;
} vec2;

typedef struct
{
	u32 id;
	i32	key;
} BinLocStrKey;

typedef struct
{
	BinHdrPtr skels;
	BinHdrPtr joints;
	BinHdrPtr bones;
	BinHdrPtr boneShapes;
	BinHdrPtr boneParts;
	BinHdrPtr bonePartTreeVals;
	BinHdrPtr rgnCells;
	BinHdrPtr stringTableBytes;
	BinHdrPtr burnGridUsedDataBytes;
	u32 itemId;
	i32 itemIdStrId;
	u32 animResId;
	vec2 iconAnimBoundsMin;
	vec2 iconAnimBoundsMax;
	BinLocStrKey name;
	i32 costCoins;
	i32 costStamps;
	i32 valueCoins;
	i32 valueStamps;
	BinLocStrKey desc;
	i32 unlisted;
	i32 popsCoins;
	i32 moneyItem;
	f32 animThreshold;
	f32 motorThreshold;
	i32 absPosition;
	f32 scaleVariance;
	i32 quantity;
	i32 shipTimeSec;
	u32 initialBurnExportId;
	i32 initialBurnPerGroup;
	i32 mouseGrabbable;
	f32 illuminate;
	i32 enableHFlip;
	i32 floorWallShadow;
	i32 splitJumpLastFrame;
	f32 purchaseCooldown;
	i32 allowDirectionalLight;
	u32 mouseGrabSoundResId;
	i32 instantEffects;
	i32 freezeOnCollide;
	i32 enableFreezePostAnim;
	u32 uniqueIgniteSoundResId;
	i32 collideItems;
	i32 collideEnvironment;
	i32 orbitalGravity;
	i32 allowExplodeStreaks;
	i32 burnSlowsAnim;
	i32 plagueOnCollide;
	u32 spawnLimitBurnExportId;
	u32 instAshSoundResId;
	i32 canGetPlague;
	i32 instAshDoesSplat;
	f32 modXAmpMin;
	f32 modXAmpMax;
	f32 modXFreqMin;
	f32 modXFreqMax;
	f32 modXPhaseMin;
	f32 modXPhaseMax;
	f32 modXSpeedMin;
	f32 modXSpeedMax;
	f32 modYAmpMin;
	f32 modYAmpMax;
	f32 modYFreqMin;
	f32 modYFreqMax;
	f32 modYPhaseMin;
	f32 modYPhaseMax;
	f32 modYSpeedMin;
	f32 modYSpeedMax;
} itemDataHeader;






//Structures for my use
typedef struct
{
	u32 size;
	uint8_t* data;
} virtualFile;

typedef struct
{
	u32 compressedSize;
	u32 decompressedSize;
	uint8_t* data;
} zlibData;

typedef struct
{
	compressedHeader cH;
	u32 id;
	bool bCompressed;
} pakHelper;

typedef struct
{
	wstring sIn;
	wstring sFilename;
	bool bCompressed;
} ThreadConvertHelper;

typedef struct
{
	wstring sFilename;
	zlibData data;
} decompressHelper;

//global functions
int compdecomp(const wchar_t* cIn, const wchar_t* cOut, int iCompress = false);	//Compress/decompress a file using zlib
int binaryToOgg( const wchar_t* in, const wchar_t* out );	//Function from Allan to convert a game sound file to .ogg
int oggToBinary( const wchar_t* in, const wchar_t* out );	//Function from Allan to convert an .ogg file to the game's sound format
takeRecord getOggData( const wchar_t* cFile );				//Grab the data from an OGG file to populate sndManifest.dat
void threadedDecompress();									//Start threaded decompression
void threadedCompress();									//Start threaded compression
//bool convertToPNG(const wchar_t* cFilename);				//Convert a game image file to PNG
bool convertToPNG(const wchar_t* cFilename, uint8_t* data, u32 size);
bool convertFromPNG(const wchar_t* cFilename);				//Convert a PNG image to a game image file
bool wordPackToXML(const wchar_t* cFilename);				//Convert wordPackDict.dat to XML
bool XMLToWordPack(const wchar_t* cFilename);				//Convert wordPackDict.dat.xml back to binary .dat form
bool sndManifestToXML(const wchar_t* cFilename);			//Convert sndManifest.dat to XML
bool XMLToSndManifest(const wchar_t* cFilename);			//Convert sndManifest.dat.xml back to binary .dat form
void initSoundManifest();									//Read in sndManifest.dat so that we can have the correct filenames for all sounds
u32 getSoundId(wstring sSound);								//Get a sound ID from the filename
wstring getSoundName(u32 soundResId);						//Get a sound filename from the sound ID
bool itemManifestToXML(const wchar_t* cFilename);			//Convert itemmanifest.dat to XML
bool XMLToItemManifest(const wchar_t* cFilename);			//Convert itemmanifest.dat.xml back to binary .dat form
void initResMap();											//Read in residmap.dat so that we can have the correct filenames for all resource files
bool residMapToXML(const wchar_t* cFilename);				//Convert residmap.dat to XML
bool XMLToResidMap(const wchar_t* cFilename);				//Convert residmap.dat.xml back to binary .dat form
const wchar_t* getName(u32 resId);							//Get a resource filename from the resource ID
u32 getResID(wstring sName);								//Get a resource ID from its filename
u32 hash(wstring sFilename);								//Hash a filename to get an ID
string ws2s(const wstring& s);								//For converting UTF-16 to UTF-8
wstring s2ws(const string& s);								//For converting UTF-8 to UTF-16
uint8_t* compress(zlibData* zIn);							//Compress via zlib
uint8_t* decompress(const zlibData* zIn);					//Decompress via zlib















#endif