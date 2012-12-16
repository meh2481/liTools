#include "pakDataTypes.h"
#include "png.h"
#include <VFS.h>
#include <VFSTools.h>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

map<u32,i32> g_IDMappings;
map<string, u32> g_repakMappings;
vector<StringTableEntry> g_stringTableList;
vector<StringPointerEntry> g_stringPointerList;
vector<char> g_stringList;
ttvfs::VFSHelper vfs;

typedef struct
{
	compressedHeader cH;
	u32 id;
	bool bCompressed;
} pakHelper;

map<string, pakHelper> g_pakHelping;

//Unravel a PNG so it can be compressed and stuffed into a .pak file
bool convertFromPNG(const char* cFilename)
{
  png_struct    *png_ptr = NULL;
  png_info      *info_ptr = NULL;
  png_byte      buf[8];
  png_byte      *png_pixels = NULL;
  png_byte      **row_pointers = NULL;
  png_uint_32   row_bytes;
  FILE 			*png_file;
  FILE 			*output_file;

  png_uint_32   width;
  png_uint_32   height;
  int           bit_depth;
  int           channels;
  int           color_type;
  int           alpha_present;
  int           ret;

  png_file = fopen(cFilename, "rb");
  if(png_file == NULL)
  {
	cout << "Unable to open " << cFilename << " for reading." << endl;
	return false;
  }
	
  // read and check signature in PNG file 
  ret = fread (buf, 1, 8, png_file);
  if (ret != 8)
  {
	cout << "Invalid read of 8" << endl;
	return false;
  }

  ret = png_sig_cmp (buf, 0, 8);
  if (ret)
  {
  cout << "sigcmp fail" << endl;
  return false;
  }

  // create png and info structures 
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
  cout << "out of memory" << endl;
  return false;   // out of memory 
  }

  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
	cout << "unable to create info struct" << endl;
    png_destroy_read_struct (&png_ptr, NULL, NULL);
    return false;   // out of memory 
  }

  if (setjmp (png_jmpbuf(png_ptr)))
  {
	cout << "setjmp failed" << endl;
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    return false;
  }

  // set up the input control for C streams 
  png_init_io (png_ptr, png_file);
  png_set_sig_bytes (png_ptr, 8);  // we already read the 8 signature bytes 

  // read the file information 
  png_read_info (png_ptr, info_ptr);

  // get size and bit depth of the PNG image 
  png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

  // set up the transformations 

  // transform paletted images into full-color rgb 
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_expand (png_ptr);
  // expand images to bit depth 8 (only applicable for grayscale images) 
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand (png_ptr);
  // transform transparency maps into full alpha channel 
  if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand (png_ptr);

  // all transformations have been registered; now update info_ptr data,
  // get rowbytes and channels, and allocate image memory 

  png_read_update_info (png_ptr, info_ptr);

  // get the new color type and bit depth (after expansion/stripping) 
  png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

  // calculate new number of channels and store alpha presence
  if (color_type == PNG_COLOR_TYPE_GRAY)
    channels = 1;
  else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    channels = 2;
  else if (color_type == PNG_COLOR_TYPE_RGB)
    channels = 3;
  else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
	channels = 4;
	png_set_bgr(png_ptr);	//Flip rgb/bgr
  }
  else
    channels = 0; // should never happen 
  alpha_present = (channels - 1) % 2;

  // check if alpha is expected to be present in file 
  if (!alpha_present)
  {
    cout << "Error:  PNG file doesn't contain alpha channel" << endl;
    return false;
  }

  // row_bytes is the width x number of channels x (bit depth / 8) 
  row_bytes = png_get_rowbytes (png_ptr, info_ptr);

  unsigned int imageSize = row_bytes * height * sizeof (png_byte);
  if ((png_pixels = (png_byte *) malloc (imageSize)) == NULL) {
	cout << "Out of memory-1" << endl;
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    return false;
  }

  if ((row_pointers = (png_byte **) malloc (height * sizeof (png_bytep))) == NULL)
  {
	cout << "Out of memory-2" << endl;
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    free (png_pixels);
    png_pixels = NULL;
    return false;
  }

  // set the individual row_pointers to point at the correct offsets 
  for (unsigned int i = 0; i < (height); i++)
    row_pointers[i] = png_pixels + i * row_bytes;

  // now we can go ahead and just read the whole image 
  png_read_image (png_ptr, row_pointers);

  // read rest of file, and get additional chunks in info_ptr
  png_read_end (png_ptr, info_ptr);

  // clean up after the read, and free any memory allocated
  png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
  
  fclose(png_file);	//Close input file

  //Write out to file
  ImageHeader ih;
  ih.width = width;
  ih.height = height;
  ih.flags = 0x01;
  
  char* cOutFilename = (char*) malloc(sizeof(char)*(strlen(cFilename) + 7));
  strcpy(cOutFilename, cFilename);
  output_file = fopen(strcat(cOutFilename, ".temp"), "wb");
  if(output_file == NULL)
  {
	cout << "output file null" << endl;
	return false;
  }
  
  if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA) //Flip image layout in memory if greyscale
  {
    ih.flags |= 0x08;	//Also change flags so it's read in correctly
	for(unsigned int i = 0; i < imageSize; i += 4)
	{
		//Shift these around so it works out right (yeah, I dunno why either, just getting it to mimic input data here)
		png_byte* curPtr = &png_pixels[i];
		png_byte temp = curPtr[3];
		curPtr[3] = curPtr[2];
		curPtr[2] = curPtr[1];
		curPtr[1] = curPtr[0];
		curPtr[0] = temp;		
	}
  }
  
  fwrite(&ih, 1, sizeof(ImageHeader), output_file);	//Write out our image header
  fwrite(png_pixels, 1, imageSize, output_file);	//and the image itself
  fclose(output_file);								//Done

  //Clean up
  if (row_pointers != (unsigned char**) NULL)
    free (row_pointers);
  if (png_pixels != (unsigned char*) NULL)
    free (png_pixels);
	
  free(cOutFilename);

  return true;
}

//Read in in our decompressed "residmap.dat" file, which includes the filenames for us to stick them in the right place
void readResidMap()
{
	FILE* fp = fopen("residmap.dat", "rb");	//This is "debug.pak" already in extracted form in the file "residmap.dat", for my ease of use.
	if(fp == NULL)
	{
		cout << "Unable to open residmap.dat. Please place this file in the same directory as this program. Abort." << endl;
		exit(0);
	}
	
	DebugPakHeader dph;
	
	//Read in the headers
	if(fread((void*)&(dph), 1, sizeof(DebugPakHeader), fp) != sizeof(DebugPakHeader))
	{
		cout << "DebugPakHeader malformed" << endl;
		fclose(fp);
		exit(0);
	}
	
	//Read in the mappings
	for(int i = 0; i < dph.maps.count; i++)
	{
		MappingHeader mh;
		if(fread((void*)&mh, 1, sizeof(MappingHeader), fp) != sizeof(MappingHeader))
		{
			cout << "MappingHeader malformed" << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_IDMappings[mh.resId] = mh.strId;
	}
	
	//Now for string table header
	StringTableHeader sth;
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), fp) != sizeof(StringTableHeader))
	{
		cout << "StringTableHeader malformed" << endl;
		fclose(fp);
		exit(0);
	}
	//Allocate memory for this many string table & pointer entries
	g_stringTableList.reserve(sth.numStrings);
	g_stringPointerList.reserve(sth.numPointers);
	g_stringList.reserve((sizeof(char) * sth.numStrings)*256);
	
	//Read in string table entries
	for(int i = 0; i < sth.numStrings; i++)
	{
		StringTableEntry ste;
		if(fread((void*)&ste, 1, sizeof(StringTableEntry), fp) != sizeof(StringTableEntry))
		{
			cout << "StringTableEntry " << i << " malformed out of " << sth.numStrings << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_stringTableList[i] = ste;
	}
	
	//and string table pointers
	for(int i = 0; i < sth.numPointers; i++)
	{
		StringPointerEntry spe;
		if(fread((void*)&spe, 1, sizeof(StringPointerEntry), fp) != sizeof(StringPointerEntry))
		{
			cout << "StringPointerEntry malformed" << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_stringPointerList[i] = spe;
	}
	
	//Now read in the strings until we hit the end of the file
	int c;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == '\\')	//Change all backslashes to forward slashes. Tsk, tsk, Allan.
			c = '/';
		g_stringList.push_back(c);
	}
	
	fclose(fp);
	
	//Reverse mappings so we can access string ID's easily from the string filename
	for(map<u32, i32>::iterator i = g_IDMappings.begin(); i != g_IDMappings.end(); i++)
	{
		i32 strId = i->second;
		u32 finalNum = i->first;
		char* cData = g_stringList.data();
		string s = &cData[g_stringPointerList[g_stringTableList[strId].pointerIndex].offset];
		g_repakMappings[s] = finalNum;
	}
}

//Remove files in the temp/ folder, recursively
void removeTempFiles()
{
	ttvfs::StringList slFiles;
    ttvfs::GetFileList("temp", slFiles);

    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    {
		string s = "temp/" + (*il);
		unlink(s.c_str());	//Remove this file
    }
	rmdir("temp/");	//Remove the folder itself
}

//Main program entry point
int main(int argc, char** argv)
{
	vfs.Prepare();
		
	//read in the resource names to pack
	readResidMap();
	
	if(argc < 2)
	{
		cout << "Usage: liCompress [pakfile1] [pakfile2] ... [pakfileN]" << endl;
		return 0;
	}
	
	for(int iArg = 1; iArg < argc; iArg++)
	{
		cout << endl << "Packing resource blob file " << argv[iArg] << endl;
		
		//Determine what files to pack into this .pak file
		list<string> lstrFilesToPak;
		string sInfilename = argv[iArg];
		sInfilename += ".filelist.txt";
		ifstream infile(sInfilename.c_str());
		if(infile.fail())
		{
			cout << "Cannot open " << sInfilename << " to pack " << argv[iArg] << " Skipping..." << endl;
			continue;
		}
		while(!infile.fail() && !infile.eof())	//Pull in all the lines out of this file
		{
			string s;
			getline(infile, s);
			if(!s.length() || s == "")
				continue;	//Ignore blank lines
			lstrFilesToPak.push_back(s);
		}
		
		//Pull in each file
		removeTempFiles();
		if(!ttvfs::IsDirectory("temp"))
			ttvfs::CreateDirRec("temp");
		int iCurFilePaking = 0;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{
			iCurFilePaking++;
			cout << "Compressing file " << iCurFilePaking << " of " << lstrFilesToPak.size() << ": " << *i << endl;
			const char* cName = i->c_str();
			u32 repakName = g_repakMappings[*i];
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", repakName);
			
			//If this was an OGG sound
			if(strstr(cName, ".flac") != NULL ||
			   strstr(cName, ".FLAC") != NULL)
			{
				string s = *i + ".ogg";
				//string o = s + ".temp";
				oggToBinary(s.c_str(), cIDFilename);
				//compdecomp(o.c_str(), cIDFilename, 1);
				g_pakHelping[*i].bCompressed = false;
				
				//g_pakHelping[*i].bCompressed = true;	//TODO: Figure out why uncompressed isn't working
				//g_pakHelping[*i].cH.uncompressedSizeBytes = ttvfs::GetFileSize(o.c_str());	//Hang onto these for compressed header stuff
				//g_pakHelping[*i].cH.compressedSizeBytes = ttvfs::GetFileSize(cIDFilename);
				//unlink(o.c_str());	//Delete temp file
			}
			//If this was a PNG image
			else if(strstr(cName, ".png") != NULL ||
			   strstr(cName, ".PNG") != NULL ||
			   strstr(cName, "coloritemicon") != NULL ||
			   strstr(cName, "colorbgicon") != NULL ||
			   strstr(cName, "greybgicon") != NULL)			//Also would include .png.normal files as well
			{
				convertFromPNG(cName);	//Do the conversion
				
				string s = *i + ".temp";	//Use the decompressed PNG for this
				compdecomp(s.c_str(), cIDFilename, 1);
				g_pakHelping[*i].bCompressed = true;
				g_pakHelping[*i].cH.uncompressedSizeBytes = ttvfs::GetFileSize(s.c_str());	//Hang onto these for compressed header stuff
				g_pakHelping[*i].cH.compressedSizeBytes = ttvfs::GetFileSize(cIDFilename);
				unlink(s.c_str());	//Remove the temporary file
			}
			else
			{
				compdecomp(i->c_str(), cIDFilename, 1);	
				g_pakHelping[*i].bCompressed = true;
				g_pakHelping[*i].cH.uncompressedSizeBytes = ttvfs::GetFileSize(i->c_str());	//Hang onto these for compressed header stuff
				g_pakHelping[*i].cH.compressedSizeBytes = ttvfs::GetFileSize(cIDFilename);
				
			}
		}
		
		//Ok, now we have all the compressed files in temp/ . Stick them in the .pak file and call it a day
		ttvfs::StringList slFiles;
		ttvfs::GetFileList("temp", slFiles);
		if(slFiles.size() != lstrFilesToPak.size())	//These should be the same
		{
			cout << "Error: size of file list: " << slFiles.size() << " differs from size of files to pak: " << lstrFilesToPak.size() << endl;
			continue;
		}
		
		//Open our output pakfile for writing
		FILE* f = fopen(argv[iArg], "wb");
		if(f == NULL)
		{
			cout << "Unable to open file " << argv[iArg] << " for writing. Skipping..." << endl;
			continue;
		}
		
		//Add the header
		blobHeader bh;
		bh.pakVersion = 0x01;
		bh.numItems = slFiles.size();
		
		fwrite(&bh, 1, sizeof(blobHeader), f);
		
		//Get the starting file pos for where we (should) be writing objects to
		size_t offsetPos = sizeof(blobHeader) + (lstrFilesToPak.size() * sizeof(resourceHeader));	
		
		//Add the table of contents
		cout << endl;
		cout << "Adding table of contents..." << endl;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{
			resourceHeader rH;
			rH.id = g_repakMappings[*i];
			g_pakHelping[*i].id = rH.id;	//Hang onto this for later
			rH.flags = 0x01;
			
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", rH.id);
			
			rH.offset = offsetPos;	//Offset
			if(g_pakHelping[*i].bCompressed)
				rH.size = ttvfs::GetFileSize(cIDFilename) + sizeof(compressedHeader);	//Size with compressed header
			else
			{
				rH.size = ttvfs::GetFileSize(cIDFilename);	//Size without compressed header
				rH.flags = 0x00;	//Set the flags to uncompressed
			}
			
			fwrite(&rH, 1, sizeof(resourceHeader), f);	//Write this to the file
			
			offsetPos += rH.size;	//Add this size to our running tally of where we are
		}
		
		//Add actual resource data
		cout << "Adding compressed files..." << endl;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{			
			pakHelper pH = g_pakHelping[*i];
			
			//Write the compressed header (Only if compressed)
			if(pH.bCompressed)
				fwrite(&(pH.cH), 1, sizeof(compressedHeader), f);
			
			//Write this file
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", pH.id);
			
			size_t fileSize = ttvfs::GetFileSize(cIDFilename);
			char* cTemp = (char*)malloc(fileSize);
			FILE* fTempIn = fopen(cIDFilename, "rb");
			if(fTempIn == NULL)
			{
				cout << "Error opening " << cIDFilename << endl;
				continue;
			}
			
			if(fread(cTemp, 1, fileSize, fTempIn) != fileSize)
			{
				cout << "Error reading from " << cIDFilename << endl;
				continue;
			}
			
			fwrite(cTemp, 1, fileSize, f);	//Write this to the file
			
			free(cTemp);
			fclose(fTempIn);
		}
		fclose(f);	//Done packing this .pak file
		removeTempFiles();
	}
	cout << "Done." << endl;
	return 0;
}
















