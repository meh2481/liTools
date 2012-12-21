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
#include <windows.h>
using namespace std;

extern list<ThreadConvertHelper> g_lThreadedResources;

ttvfs::VFSHelper vfs;

//Remove all files in the temp/ folder, since we're done with them
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

//Save a PNG file from decompressed data
bool convertPNG(const char* cFilename)
{
  ImageHeader ih;
  FILE          *png_file;
  FILE          *input_file;
  png_struct    *png_ptr = NULL;
  png_info      *info_ptr = NULL;
  png_byte      *png_pixels = NULL;
  png_byte      **row_pointers = NULL;
  png_uint_32   row_bytes;

  int           color_type = PNG_COLOR_TYPE_RGB_ALPHA;
  int           bit_depth = 8;
  int           channels = 4;
  
  char cTempFilename[512];
  sprintf(cTempFilename, "%s.temp", cFilename);
  
  png_file = fopen(cTempFilename, "wb");
  if(png_file == NULL)
  {
    cout << "PNG file " << cTempFilename << " NULL" << endl;
	return false;
  }
  
  input_file = fopen(cFilename, "rb");
  if(input_file == NULL)
  {
    cout << "input file NULL" << endl;
	return false;
  }
	
  //Read in the image header
  if(fread((void*)&ih, 1, sizeof(ImageHeader), input_file) != sizeof(ImageHeader))
  {
    cout << "Header null" << endl;
	fclose(input_file);
	fclose(png_file);
	return false;
  }
  
  //Read in the image
  size_t sizeToRead = ih.width * ih.height * channels * bit_depth/8;
  png_pixels = (png_byte*)malloc(sizeToRead);
  size_t sizeRead = fread((void*)png_pixels, 1, sizeToRead, input_file);
  if(sizeRead != sizeToRead)
  {
    cout << "Image null: Should have read " << sizeToRead << " bytes, only read " << sizeRead << " bytes" << endl;
	fclose(input_file);
	fclose(png_file);
	return false;
  }
  
  //If this is greyscale, move alpha over so it works
  if(ih.flags & GREYSCALE_PNG)
  {
	//Same size of image, so we don't have to worry about doing this before reading it in
    color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
	bit_depth = 16;
	channels = 2;
	//Since the data is in ACCA format, change it to CCAA format
	for(unsigned int i = 0; i < sizeToRead; i += 4)
	{
		png_byte* curPtr = &png_pixels[i];
		png_byte temp = curPtr[0];
		curPtr[0] = curPtr[2];
		curPtr[2] = temp;
		
		//Also flip bytes for alpha
		temp = curPtr[2];
		curPtr[2] = curPtr[3];
		curPtr[3] = temp;
		
		//And flip bytes for color
		temp = curPtr[0];
		curPtr[0] = curPtr[1];
		curPtr[1] = temp;
	}
  }
  

  // prepare the standard PNG structures 
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    cout << "png_ptr Null" << endl;
	fclose(input_file);
	fclose(png_file);
	return false;
  }
	
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    cout << "Info ptr null" << endl;
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	fclose(input_file);
	fclose(png_file);
	return false;
  }

  // setjmp() must be called in every function that calls a PNG-reading libpng function
  if (setjmp (png_jmpbuf(png_ptr)))
  {
    cout << "unable to setjmp" << endl;
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	fclose(input_file);
	fclose(png_file);
	return false;
  }
  
  png_set_bgr(png_ptr);	//Flip rgb/bgr

  // initialize the png structure
  png_init_io (png_ptr, png_file);

  // we're going to write more or less the same PNG as the input file
  png_set_IHDR (png_ptr, info_ptr, ih.width, ih.height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // write the file header information
  png_write_info (png_ptr, info_ptr);

  // if needed we will allocate memory for an new array of row-pointers
  if (row_pointers == (unsigned char**) NULL)
  {
    if ((row_pointers = (png_byte **) malloc (ih.height * sizeof (png_bytep))) == NULL)
    {
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	  cout << "Error allocating row pointers" << endl;
	  fclose(input_file);
	  fclose(png_file);
	  return false;
    }
  }

  // row_bytes is the width x number of channels x (bit-depth / 8)
  row_bytes = ih.width * channels * ((bit_depth <= 8) ? 1 : 2);
  
  // set the individual row_pointers to point at the correct offsets
  for (unsigned int i = 0; i < (ih.height); i++)
    row_pointers[i] = png_pixels + i * row_bytes;

  // write out the entire image data in one call
  png_write_image (png_ptr, row_pointers);

  // write the additional chuncks to the PNG file (not really needed)
  png_write_end (png_ptr, info_ptr);

  // clean up after the write, and free any memory allocated
  png_destroy_write_struct (&png_ptr, (png_infopp) NULL);

  if (row_pointers != (unsigned char**) NULL)
    free (row_pointers);
  if (png_pixels != (unsigned char*) NULL)
    free (png_pixels);
	
  //Close the files
  fclose(input_file);
  fclose(png_file);
  
  if(unlink(cFilename))	//Delete old file
	cout << "error unlinking " << cFilename << endl;
  if(rename(cTempFilename, cFilename))	//Move this over the old one
	cout << "Error renaming " << cTempFilename << endl;

  return true;
}

//Create the folder that this resource ID's file will be placed in
void makeFolder(u32 resId)
{
	const char* cName = getName(resId);
	for(int i = strlen(cName)-1; i >= 0; i--)
	{
		if(cName[i] == '/')
		{
			char* cFilename = (char*)malloc(i+5);
			memset(cFilename, 0, i+5);
			strncpy(cFilename, cName, i+1);
			//char cData[512];
			//sprintf(cData, "output/%s", cFilename);
			if(!ttvfs::IsDirectory(cFilename))
				ttvfs::CreateDirRec(cFilename);
			free(cFilename);
		}
	}
}

//Main program entry point
int main(int argc, char** argv)
{
	DWORD iTicks = GetTickCount();	//Store the starting number of milliseconds
	
	vfs.Prepare();
		
	//read in the resource names to unpack
	readResidMap();
	
	if(argc < 2)
	{
		cout << "Usage: liDecompress [filename1] [filename2] ... [filenameN]" << endl;
		return 0;
	}
	
	for(int iArg = 1; iArg < argc; iArg++)
	{
		cout << endl << "Unpacking resource blob file " << argv[iArg] << endl;
		FILE* f = fopen(argv[iArg], "rb");
		if(f == NULL)
		{
			cout << "Unable to open file " << argv[iArg] << endl;
			continue;
		}
		
		blobHeader bH;
		if(fread((void*)&bH, 1, sizeof(blobHeader), f) != sizeof(blobHeader))
		{
			cout << "Error reading number of resources in file " << argv[iArg] << endl;
			fclose(f);
			continue;
		}
		
		list<resourceHeader> lResourceHeaders;
		
		for(int i = 0; i < bH.numItems; i++)
		{
			resourceHeader rH;
			size_t sizeRead = fread((void*)&rH, 1, sizeof(resourceHeader), f);
			if(sizeRead != sizeof(resourceHeader))
			{
				cout << "Read " << sizeRead << " bytes, which differs from resource header size " << sizeof(resourceHeader) << endl;
				fclose(f);
				continue;
			}
			lResourceHeaders.push_back(rH);
		}
		
		//Create temp folder if it isn't here already
		removeTempFiles();
		if(!ttvfs::IsDirectory("temp"))
			ttvfs::CreateDirRec("temp");
			
		//Create list file with all the files that were in this .pak
		string sPakListFilename = "";
		for(int i = strlen(argv[iArg])-1; i >= 0; i--)
		{
			if(argv[iArg][i] == '\\' ||
			   argv[iArg][i] == '/')
				break;
			sPakListFilename.insert(sPakListFilename.begin(), argv[iArg][i]);
		}
		sPakListFilename += ".filelist.txt";
		ofstream oPakList(sPakListFilename.c_str());
		
		//Iterate through these items, splitting them out of the file and creating new files out of each
		cout << "Extracting files..." << endl;
		for(list<resourceHeader>::iterator i = lResourceHeaders.begin(); i != lResourceHeaders.end(); i++)
		{
			ThreadConvertHelper tch;
			tch.bCompressed = false;
			makeFolder(i->id);
			const char* cName = getName(i->id);
			oPakList << cName << endl;
			fseek(f, i->offset, SEEK_SET);
			tch.sFilename = cName;
			if(i->flags == FLAG_ZLIBCOMPRESSED)
			{
				compressedHeader cH;
				if(fread((void*)&cH, 1, sizeof(compressedHeader), f) != sizeof(compressedHeader))
				{
					cout << "Error reading compressed header." << endl;
					fclose(f);
					continue;
				}
				
				char sOutFile[256];
				sprintf(sOutFile, "temp/%u", i->id);
				FILE* fOut = fopen(sOutFile, "wb");
				
				uint32_t size = cH.compressedSizeBytes;
				
				uint8_t* buf = (uint8_t*)malloc(size);
				size_t sizeRead = fread((void*)buf, 1, size, f);
				if(sizeRead != size)
				{
					cout << "Error reading compressed data. Size: " << size << " read: " << sizeRead << endl;
					fclose(f);
					fclose(fOut);
					free(buf);
					continue;
				}
				fwrite((void*)buf, 1, size, fOut);
				fclose(fOut);
				free(buf);
				
				tch.sIn = sOutFile;
				tch.bCompressed = true;
			}
			else if(i->flags == FLAG_NOCOMPRESSION)
			{
				tch.sIn = "";
				FILE* fOut = fopen(cName, "wb");
				uint8_t* buf = (uint8_t*)malloc(i->size);
			  
				if(fread((void*)buf, 1, i->size, f) != i->size)
				{
					cout << "Error reading non-compressed data." << endl;
					fclose(f);
					fclose(fOut);
					free(buf);
					continue;
				}
				fwrite((void*)buf, 1, i->size, fOut);
				
			  
				free(buf);
				fclose(fOut);
			}
			else
			{
				cout << "Invalid resource flag " << i->flags << endl;
			}
			
			g_lThreadedResources.push_back(tch);
			
		}
		
		threadedDecompress();
		
		fclose(f);
		oPakList.close();
	}
	
	removeTempFiles();
	cout << "Done." << endl;
	
	iTicks = GetTickCount() - iTicks;
	int iSeconds = iTicks / 1000;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	
	return 0;
}
