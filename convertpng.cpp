#include "pakDataTypes.h"
#include "png.h"

//Save a PNG file from decompressed data
bool convertToPNG(const wchar_t* cFilename, uint8_t* data, u32 size)
{
  ImageHeader ih;
  FILE          *png_file;
  png_struct    *png_ptr = NULL;
  png_info      *info_ptr = NULL;
  png_byte      *png_pixels = NULL;
  png_byte      **row_pointers = NULL;
  png_uint_32   row_bytes;

  int           color_type = PNG_COLOR_TYPE_RGB_ALPHA;
  int           bit_depth = 8;
  int           channels = 4;
  
  png_file = _wfopen(cFilename, TEXT("wb"));
  if(png_file == NULL)
  {
    cout << "PNG file " << ws2s(cFilename) << " NULL" << endl;
	return false;
  }
  setvbuf ( png_file , NULL , _IOFBF , 4096 );
  
  //Read in the image header
  memcpy((void*)&ih, data, sizeof(ImageHeader));
  
  //Read in the image
  size_t sizeToRead = ih.width * ih.height * channels * bit_depth/8;
  png_pixels = &data[sizeof(ImageHeader)];
  
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
		
		//Divide alpha out of premultiplied image
		unsigned short col, alpha;
		alpha = curPtr[3] + ((unsigned short)curPtr[2] << 8);
		col = curPtr[1] + ((unsigned short)curPtr[0] << 8);
		col /= (float)alpha / (float)0xFFFF;
		curPtr[1] = col & 0xFF;
		curPtr[0] = col >> 8;
	}
  }
  else	//Color image; change premultiplied alpha to normal alpha
  {
	for(unsigned int i = 0; i < sizeToRead; i += 4)
	{
		png_byte* curPtr = &png_pixels[i];
		
		//Divide alpha
		if(curPtr[3] == 0)
			curPtr[0] = curPtr[1] = curPtr[2] = 0;
		else
		{
			float fAlpha = (float)curPtr[3] / 255.0;
			curPtr[0] /= fAlpha;
			curPtr[1] /= fAlpha;
			curPtr[2] /= fAlpha;
		}			
	}
  }
  

  // prepare the standard PNG structures 
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    cout << "png_ptr Null" << endl;
	fclose(png_file);
	return false;
  }
	
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    cout << "Info ptr null" << endl;
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	fclose(png_file);
	return false;
  }

  // setjmp() must be called in every function that calls a PNG-reading libpng function
  if (setjmp (png_jmpbuf(png_ptr)))
  {
    cout << "unable to setjmp" << endl;
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
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
  if (row_pointers == (png_byte**) NULL)
  {
    if ((row_pointers = (png_byte **) malloc (ih.height * sizeof (png_bytep))) == NULL)
    {
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	  cout << "Error allocating row pointers" << endl;
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

  if (row_pointers != (png_byte**) NULL)
    free (row_pointers);
	
  //Close the file
  fclose(png_file);

  return true;
}

size_t wstrlen(const wchar_t* s)
{
	for(unsigned int i = 0; ; i++)
	{
		if(s[i] == L'\0')
			return i+1;
	}
	return 0;
}

void wstrcpy(wchar_t* cOut, const wchar_t* cIn)
{
	for(unsigned int i = 0; ; i++)
	{
		cOut[i] = cIn[i];
		if(cIn[i] == L'\0')
			break;
	}
}

//Unravel a PNG so it can be compressed and stuffed into a .pak file
bool convertFromPNG(const wchar_t* cFilename)
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

  png_file = _wfopen(cFilename, TEXT("rb"));
  if(png_file == NULL)
  {
	cout << "Unable to open " << cFilename << " for reading." << endl;
	return false;
  }
  setvbuf ( png_file , NULL , _IOFBF , 4096 );
	
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
  cout << "sigcmp fail. This is not a PNG image." << endl;
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
	cout << "Out of memory for PNG pixels" << endl;
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    return false;
  }

  if ((row_pointers = (png_byte **) malloc (height * sizeof (png_bytep))) == NULL)
  {
	cout << "Out of memory for PNG row pointers" << endl;
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
  
  wchar_t* cOutFilename = (wchar_t*) malloc(sizeof(wchar_t)*(wstrlen(cFilename) + 7));
  wstrcpy(cOutFilename, cFilename);
  wstring sTemp = cOutFilename;
  sTemp += TEXT(".temp");
  output_file = _wfopen(sTemp.c_str(), TEXT("wb"));
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
		png_byte* curPtr = &png_pixels[i];
		
		//Convert to premultiplied alpha
		unsigned short col, alpha;
		alpha = curPtr[3] + ((unsigned short)curPtr[2] << 8);
		col = curPtr[1] + ((unsigned short)curPtr[0] << 8);
		col *= (float)alpha / (float)0xFFFF;
		curPtr[1] = col & 0xFF;
		curPtr[0] = col >> 8;
		
		//Shift these around so it works out right (yeah, I dunno why either. I'm just getting it to mimic input data here)
		png_byte temp = curPtr[3];
		curPtr[3] = curPtr[2];
		curPtr[2] = curPtr[1];
		curPtr[1] = curPtr[0];
		curPtr[0] = temp;
		
	}
  }
  else	//Color image; convert to premultiplied alpha
  {
	for(unsigned int i = 0; i < imageSize; i += 4)
	{
		png_byte* curPtr = &png_pixels[i];
		float fAlpha = (float)curPtr[3] / 255.0;
		curPtr[0] *= fAlpha;
		curPtr[1] *= fAlpha;
		curPtr[2] *= fAlpha;
		
		if(curPtr[3] == 0)
			curPtr[0] = curPtr[1] = curPtr[2] = 0;
	}
  }
  
  fwrite(&ih, 1, sizeof(ImageHeader), output_file);	//Write out our image header
  fwrite(png_pixels, 1, imageSize, output_file);	//and the image itself
  fclose(output_file);								//Done

  //Clean up
  if (row_pointers != (png_byte**) NULL)
    free (row_pointers);
  if (png_pixels != (png_byte*) NULL)
    free (png_pixels);
	
  free(cOutFilename);

  return true;
}
