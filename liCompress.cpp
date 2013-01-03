#include "pakDataTypes.h"

ttvfs::VFSHelper vfs;

extern list<wstring> g_lThreadedResources;	//Declared in threadCompress.cpp, used for multi-threaded compression
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
			g_lThreadedResources.push_back(s);	//Add this to our list of files to package
		}
		list<wstring> lFilenames = g_lThreadedResources;	//Copy this to use later
		
		threadedCompress();	//Compress everything
		
		//Ok, now we have all the compressed files in RAM. Stick them in the .pak file and call it a day
		if(g_pakHelping.size() != lFilenames.size())	//These should be the same
		{
			cout << "Error: size of file list: " << g_pakHelping.size() << " differs from size of files to pak: " << lFilenames.size() << endl;
			continue;
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
			rH.id = getResID(*i);
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
















