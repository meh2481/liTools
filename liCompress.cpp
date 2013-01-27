#include "pakDataTypes.h"

ttvfs::VFSHelper vfs;

extern bool g_bProgressOverwrite;
extern unsigned int g_iNumThreads;
extern int g_iCompressAmount;

map<wstring, pakHelper> g_pakHelping;

void parseCmdLine(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		if(s == "--overwrite-progress")
			g_bProgressOverwrite = true;
		else if(s.find("--threads=") != wstring::npos)
		{
			size_t pos = s.find(L'=')+1;
			if(s.length() <= pos)
			{
				cout << "missing thread count. Ignoring..." << endl;
				continue;
			}
			int iNumThreads = atoi(&s.c_str()[pos]);
			if(iNumThreads < 0 ||
			   iNumThreads > MAX_NUM_THREADS)
			{
				cout << "Invalid number of threads: " << iNumThreads << ". Ignoring..." << endl;
				continue;
			}
			g_iNumThreads = iNumThreads;
		}
		else if(s.find("--level=") != wstring::npos)
		{
			size_t pos = s.find(L'=')+1;
			if(s.length() <= pos)
			{
				cout << "Missing compression level. Ignoring..." << endl;
				continue;
			}
			int iCompressLevel = atoi(&s.c_str()[pos]);
			if(iCompressLevel < Z_DEFAULT_COMPRESSION ||
			   iCompressLevel > Z_BEST_COMPRESSION)
			{
				cout << "Invalid compression level: " << iCompressLevel << ". Ignoring..." << endl;
				continue;
			}
			g_iCompressAmount = iCompressLevel;
		}
		else if(argv[i][0] == '-')
			cout << "Unknown commandline switch " << argv[i] << ". Ignoring..." << endl;
	}
}

void createMiniResidMap(map<u32,wstring> *mappings)
{
	//Fill in the residmap data
	list<char> lUTFData;
	list<MappingHeader> lMappings;
	list<StringTableEntry> lStringTable;
	list<StringPointerEntry> lStringPointers;
	for(map<u32,wstring>::iterator i = mappings->begin(); i != mappings->end(); i++) 
	{
		int id = i->first;
		const char* cName = ws2s(i->second).c_str();
		//Make mapping header that maps this resource ID to the string ID
		MappingHeader mh;
		mh.resId = id;
		mh.strId = lStringTable.size();
		lMappings.push_back(mh);
		//Make StringTableEntry that maps this string ID to a string data pointer
		StringTableEntry ste;
		ste.pointerIndex = lStringPointers.size();
		ste.pointerCount = 1;
		lStringTable.push_back(ste);
		//Make the StringPointerEntry that maps this pointer to a location in the string data
		StringPointerEntry spe;
		spe.languageId = LANGID_ENGLISH;
		spe.offset = lUTFData.size();
		lStringPointers.push_back(spe);
		//Add this string to our string list
		unsigned int iStrLen = strlen(cName)+1;	//+1 so we can keep the terminating \0 character
		for(unsigned int i = 0; i < iStrLen; i++)
			lUTFData.push_back(cName[i]);			//Copy data over
	}
		
	//Write everything to a pakHelper virtual "file"	
	pakHelper pH;
	pH.bCompressed = false;
	pH.dataSz = sizeof(ResidMapHeader) + 
				sizeof(MappingHeader) * lMappings.size() +
				sizeof(StringTableHeader) +
				sizeof(StringTableEntry) * lStringTable.size() +
				sizeof(StringPointerEntry) * lStringPointers.size() +
				lUTFData.size();
	pH.data = (uint8_t*)malloc(pH.dataSz);
	
	//Write out our ResidMapHeader
	ResidMapHeader rmh;
	size_t curOffset = sizeof(ResidMapHeader);
	rmh.maps.count = lMappings.size();
	rmh.maps.offset = curOffset;
	curOffset += sizeof(MappingHeader) * lMappings.size();
	//The count for this is the number of bytes for all of it
	rmh.stringTableBytes.count = sizeof(StringTableHeader) + sizeof(StringTableEntry) * lStringTable.size() + lUTFData.size();
	rmh.stringTableBytes.offset = curOffset;
	
	curOffset = 0;	//Use this from now on to keep track of where we're writing
	memcpy(&pH.data[curOffset], (uint8_t*)&rmh, sizeof(ResidMapHeader));
	curOffset += sizeof(ResidMapHeader);
	//fwrite(&rmh, 1, sizeof(ResidMapHeader), f);
	
	//Write out our MappingHeaders
	for(list<MappingHeader>::iterator i = lMappings.begin(); i != lMappings.end(); i++)
	{
		//fwrite(&(*i), 1, sizeof(MappingHeader), f);
		memcpy(&pH.data[curOffset], (uint8_t*)&(*i), sizeof(MappingHeader));
		curOffset += sizeof(MappingHeader);
	}
		
	//Write out our StringTableHeader
	StringTableHeader sth;
	sth.numStrings = lStringTable.size();
	sth.numPointers = lStringPointers.size();
	//fwrite(&sth, 1, sizeof(StringTableHeader), f);
	memcpy(&pH.data[curOffset], (uint8_t*)&sth, sizeof(StringTableHeader));
	curOffset += sizeof(StringTableHeader);
	
	//Write out our StringTableEntries
	for(list<StringTableEntry>::iterator i = lStringTable.begin(); i != lStringTable.end(); i++)
	{
		//fwrite(&(*i), 1, sizeof(StringTableEntry), f);
		memcpy(&pH.data[curOffset], (uint8_t*)&(*i), sizeof(StringTableEntry));
		curOffset += sizeof(StringTableEntry);
	}
	
	//Write out our StringPointerEntries
	for(list<StringPointerEntry>::iterator i = lStringPointers.begin(); i != lStringPointers.end(); i++)
	{
		//fwrite(&(*i), 1, sizeof(StringPointerEntry), f);
		memcpy(&pH.data[curOffset], (uint8_t*)&(*i), sizeof(StringPointerEntry));
		curOffset += sizeof(StringPointerEntry);
	}
	
	//Write out our string data
	for(list<char>::iterator i = lUTFData.begin(); i != lUTFData.end(); i++)
	{
		//fwrite(&(*i), 1, 1, f);
		memcpy(&pH.data[curOffset], (uint8_t*)&(*i), 1);
		curOffset++;
	}
	
	g_pakHelping[TEXT(RESIDMAP_NAME)] = pH;	//Done
}

//Main program entry point
int main(int argc, char** argv)
{
	g_bProgressOverwrite = false;
	g_iNumThreads = 0;
	g_iCompressAmount = Z_DEFAULT_COMPRESSION;
	DWORD iTicks = GetTickCount();

	vfs.Prepare();
		
	//read in the resource names to pack
	initResMap();
	initSoundManifest();
	parseCmdLine(argc, argv);
	
	if(argc < 2)
	{
		cout << "Usage: liCompress [pakfile1] [pakfile2] ... [pakfileN]" << endl;
		return 0;
	}
	
	for(int iArg = 1; iArg < argc; iArg++)
	{
		if(argv[iArg][0] == '-')	//Skip over commandline switches
			continue;
		
		list<wstring> lFilenames;	//Files in this pakfile that we're going to compress
		wstring sArg = s2ws(argv[iArg]);
		size_t pos = sArg.find(TEXT(".filelist.txt"));
		if(pos != wstring::npos)
			sArg.erase(pos, wstring::npos);	//Erase a .filelist.pak extension if there is one
		cout << endl << "Packing resource blob file " << ws2s(sArg) << endl;
		
		//Determine what files to pack into this .pak file
		wstring sInfilename = sArg;
		sInfilename += TEXT(".filelist.txt");
		ifstream infile(ws2s(sInfilename).c_str());
		if(infile.fail())
		{
			cout << "Cannot open " << ws2s(sInfilename) << " to pack " << ws2s(sArg) << " Skipping..." << endl;
			continue;
		}
		
		while(!infile.fail() && !infile.eof())	//Pull in all the lines out of this file
		{
			string ss;
			getline(infile, ss);
			wstring s = s2ws(ttvfs::FixSlashes(ss));
			if(!s.length() || s == TEXT(""))
				continue;	//Ignore blank lines
			lFilenames.push_back(s);	//Add this to our list of files to package
		}
		
		threadedCompress(lFilenames);	//Compress everything
		
		//Ok, now we have all the compressed files in RAM. Stick them in the .pak file and call it a day
		if(g_pakHelping.size() != lFilenames.size())	//These should be the same
		{
			cout << "Error: size of file list: " << g_pakHelping.size() << " differs from size of files to pak: " << lFilenames.size() << endl;
			continue;
		}
		
		//Create a mini residmap.dat file and stick it into g_pakHelping to compress if we need to
		//See what IDs are known and unknown
		map<wstring, u32> mResIDs;
		map<u32, wstring> mUnknownIDs;
		for(list<wstring>::iterator i = lFilenames.begin(); i != lFilenames.end(); i++)
		{
			u32 id = getKnownResID(*i);
			if(!id)	//The ID mapping is unknown
			{
				id = hash(*i);
				mUnknownIDs[id] = toBackslashes(*i);	//Make sure we use backslashes inside a residmap.dat
			}
			mResIDs[*i] = id;
		}
		
		//If there are any unknown mappings, make our residmap.dat
		if(mUnknownIDs.size())
		{
			lFilenames.push_front(TEXT(RESIDMAP_NAME));	//Add this to the front, so it'll decompress first so we'll have all the filenames
			mResIDs[TEXT(RESIDMAP_NAME)] = RESIDMAP_ID;	//Add this to the IDs we'll compress
			createMiniResidMap(&mUnknownIDs);	//And create the file in memory
		}
		
		//Open our output pakfile for writing
		FILE* f = _wfopen(sArg.c_str(), TEXT("wb"));
		if(f == NULL)
		{
			cout << "Unable to open file " << ws2s(sArg) << " for writing. Skipping..." << endl;
			continue;
		}
		
		//Add the header
		blobHeader bh;
		bh.pakVersion = 0x01;
		bh.numItems = lFilenames.size();
		
		fwrite(&bh, 1, sizeof(blobHeader), f);
		
		//Get the starting file pos for where we (should) be writing objects to
		size_t offsetPos = sizeof(blobHeader) + (lFilenames.size() * sizeof(resourceHeader));	
		
		//Add the table of contents
		cout << "\rAdding table of contents...                " << endl;
		for(list<wstring>::iterator i = lFilenames.begin(); i != lFilenames.end(); i++)
		{
			resourceHeader rH;
			rH.id = mResIDs[*i];
			rH.flags = 0x01;
			
			rH.offset = offsetPos;	//Offset
			rH.size = g_pakHelping[*i].dataSz;
			if(g_pakHelping[*i].bCompressed)
				rH.size += sizeof(compressedHeader);	//Compressed files have a compression header also
			else
				rH.flags = 0x00;	//Set the flags to uncompressed
			
			fwrite(&rH, 1, sizeof(resourceHeader), f);	//Write this to the file
			
			offsetPos += rH.size;	//Add this size to our running tally of where we are
		}
		
		//Add actual resource data
		cout << "Adding compressed files..." << endl;
		for(list<wstring>::iterator i = lFilenames.begin(); i != lFilenames.end(); i++)
		{
			pakHelper pH = g_pakHelping[*i];
			
			//Write the compressed header only if compressed
			if(pH.bCompressed)
				fwrite(&(pH.cH), 1, sizeof(compressedHeader), f);
			
			fwrite(pH.data, 1, pH.dataSz, f);	//One pass to write this file. Simple enough.
			//Don't free the memory here, in case there's more than one .pak file with this data in it.
		}
		fclose(f);	//Done packing this .pak file
		
		//Clear memory
		for(map<wstring, pakHelper>::iterator i = g_pakHelping.begin(); i != g_pakHelping.end(); i++)
			free(i->second.data);
		g_pakHelping.clear();
	}
	
	cout << "Done." << endl;
	
	iTicks = GetTickCount() - iTicks;
	float iSeconds = (float)iTicks / 1000.0;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	
	return 0;
}
















