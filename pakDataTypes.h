//What we were given from Allan

#ifndef LIDECOMPRESS_H
#define LIDECOMPRESS_H

#include <cstdio>
#include <stdint.h>
#include <string>
#include <list>
using namespace std;
#define i32 int32_t
#define u32 uint32_t
#define f32	float

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
} DebugPakHeader;

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


//helper functions
int compdecomp(const char* cIn, const char* cOut, int iCompress = false);
int binaryToOgg( const char* in, const char* out );
int oggToBinary( const char* in, const char* out );
takeRecord getOggData( const char* cFile );
void threadedDecompress();
void threadedCompress();
bool convertPNG(const char* cFilename);
bool convertFromPNG(const char* cFilename);
bool wordPackToXML(const char* cFilename);
bool XMLToWordPack(const char* cFilename);
bool sndManifestToXML(const char* cFilename);
bool XMLToSndManifest(const char* cFilename);
void readResidMap();
const char* getName(u32 resId);
u32 getResID(string sName);


#endif