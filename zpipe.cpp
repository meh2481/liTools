#include "pakDataTypes.h"

int g_iCompressAmount;

// compress from the specified buffer, returning the compressed buffer
uint8_t* compress(zlibData* zIn)
{
    uint8_t* ret;
    //Compress
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (deflateInit(&strm, g_iCompressAmount) != Z_OK)
		return NULL;
	strm.avail_in = zIn->decompressedSize;
	strm.next_in = zIn->data;
	strm.avail_out = zIn->decompressedSize;
	ret = (uint8_t*) malloc(zIn->decompressedSize);	//Allocate enough memory for uncompressed size. If we go over this, ignore
	strm.next_out = ret;
	int ok = deflate(&strm, Z_FINISH);
	if(ok != Z_STREAM_END)							//Error, or out of memory
	{
		if(ok != Z_OK)	//== Z_OK means ran out of space and we shouldn't compress this resource
			cout << "zlib deflate error: " << ok << endl;
		(void)deflateEnd(&strm);
		free(ret);	//Clear this memory, since we won't be using it
		return NULL;
	}
	zIn->compressedSize = zIn->decompressedSize - strm.avail_out;	//Save the size that we compressed to
	(void)deflateEnd(&strm);	//Done deflating
	return ret;
}

//Decompress from the specified input buffer, returning the uncompressed data
uint8_t* decompress(const zlibData* zIn)
{
	uint8_t* ret;
	//Decompress
	z_stream strm;
	strm.zalloc = Z_NULL;	//Use default memory allocation
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = zIn->compressedSize;		//How large our input buffer is
	strm.avail_out = zIn->decompressedSize;	//How large our output buffer is
    strm.next_in = Z_NULL;
    if (inflateInit(&strm) != Z_OK)			//Initialize decompression
        return NULL;
	strm.next_in = zIn->data;
	ret = (uint8_t*)malloc(zIn->decompressedSize);	//Allocate memory to hold our decompressed data
	strm.next_out = ret;							//And point decompressor at it
	int ok = inflate(&strm, Z_NO_FLUSH);			//Decompress
	switch (ok)
	{
		case Z_NEED_DICT:
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&strm);
			cout << "zlib inflate error: " << ok << endl;
			return NULL;
	}
	(void)inflateEnd(&strm);	//Done	
	return ret;
}









