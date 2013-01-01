#include "pakDataTypes.h"

#define RESOURCE_1_NAME	TEXT("resource.pak")
#define RESOURCE_2_NAME	TEXT("embed.pak")
#define RESOURCE_3_NAME	TEXT("frontend.pak")

typedef struct
{
	uint16_t size;
	uint8_t* data;
} virtualFile;

map<u32, virtualFile> g_mOrig;
map<u32, virtualFile> g_mMods;
ofstream oWarnings("mergeresults.txt");

ttvfs::VFSHelper vfs;

//Functions from Stack Overflow peoples
wstring s2ws(const string& s)
{
    int len;
    int slength = (int)s.length();
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wstring r(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
    return r;
}

string ws2s(const wstring& s)
{
    int len;
    int slength = (int)s.length();
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
    string r(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0); 
    return r;
}

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
	
	//Remove tempmod files too
	ttvfs::GetFileList("tempmod", slFiles);

    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    {
		string s = "tempmod/" + (*il);
		unlink(s.c_str());	//Remove this file
    }
	rmdir("tempmod/");	//Remove the folder itself
}

void splitOutFiles(FILE* f, list<resourceHeader>* lRH, bool bMod)
{
	//Iterate through these items, splitting them out of the file and saving the data
	for(list<resourceHeader>::iterator i = lRH->begin(); i != lRH->end(); i++)
	{
		fseek(f, i->offset, SEEK_SET);
		
		//char sOutFile[256];
		
		//FILE* fOut = fopen(sOutFile, "wb");
		uint8_t* buf = (uint8_t*)malloc(i->size);
	  
		if(fread((void*)buf, 1, i->size, f) != i->size)
		{
			cout << "Error reading non-compressed data." << endl;
			fclose(f);
			//fclose(fOut);
			free(buf);
			continue;
		}
		//fwrite((void*)buf, 1, i->size, fOut);
		virtualFile vf;
		vf.size = i->size;
		vf.data = buf;
		if(bMod)
		{
			if(g_mMods.count(i->id))	//This was here already
			{
				oWarnings << "Conflict with mod ID " << i->id << ". Overwriting..." << endl;
				free(g_mMods[i->id].data);	//Clean up original memory
			}
			g_mMods[i->id] = vf;
		}
		else
		{
			if(g_mOrig.count(i->id))	//This was here already
			{
				oWarnings << "Conflict with original ID " << i->id << ". Overwriting..." << endl;
				free(g_mOrig[i->id].data);	//Clean up original memory
			}
			g_mOrig[i->id] = vf;
		}
		//free(buf);
		//fclose(fOut);
	}
}

void copyTempFiles()
{
	for(map<u32, virtualFile>::iterator i = g_mMods.begin(); i != g_mMods.end(); i++)
	{
		
	}

	//ttvfs::StringList slFiles;
    //ttvfs::GetFileList("tempmod", slFiles);

    //for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    //{
	//	string s = "tempmod/" + (*il);
	//	string sOut = "temp/" + (*il);
	//	unlink(sOut.c_str());	//Remove the old file
	//	rename(s.c_str(), sOut.c_str());	//Replace it with this one
    //}
}

//Main program entry point
int main(int argc, char** argv)
{
	DWORD iTicks = GetTickCount();	//Store the starting number of milliseconds
	
	vfs.Prepare();
	
	if(argc < 2)
	{
		cout << "Usage: modManage [pakfile1] [pakfile2] ... [pakfileN]" << endl;
		return 0;
	}
	
	//Create temp folders if they aren't here already
	/*removeTempFiles();
	if(!ttvfs::IsDirectory("temp"))
		ttvfs::CreateDirRec("temp");
	if(!ttvfs::IsDirectory("tempmod"))
		ttvfs::CreateDirRec("tempmod");*/
	
	//First, check and see what resource ID's are in each mod to merge, and split them all out
	list<resourceHeader> lModResHeaders[argc-1];	//For splitting mods out of the .pak files
	map<u32, u32> mModHeader;	//For repacking, need to know the flags for this file
	map<u32, bool> mModResources;
	//cout << endl;
	for(int iArg = 1; iArg < argc; iArg++)
	{
		//Read in these files
		cout << "Unpacking mod " << argv[iArg] << endl;
		FILE* f = _wfopen(s2ws(argv[iArg]).c_str(), TEXT("rb"));
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
			lModResHeaders[iArg-1].push_back(rH);
			mModResources[rH.id] = true;
			mModHeader[rH.id] = rH.flags;
		}
		
		splitOutFiles(f, &lModResHeaders[iArg-1], true);
		
		fclose(f);
	}
	
	bool bPack[3];
	list<resourceHeader> lResourceHeaders[3];
	
	//Now that we have all the resources from the mod .pak files unpacked, check and see what original .pak files need to be changed
	for(int iPak = 0; iPak < 3; iPak++)
	{
		wstring sArg;
		
		switch(iPak)
		{
			case 0:
				sArg = RESOURCE_1_NAME;
				break;
			case 1:
				sArg = RESOURCE_2_NAME;
				break;
			default:
				sArg = RESOURCE_3_NAME;
				break;
		}
		
		cout << "Pulling headers from resource blob file " << ws2s(sArg) << endl;
		FILE* f = _wfopen(sArg.c_str(), TEXT("rb"));
		if(f == NULL)
		{
			cout << "Unable to open file " << ws2s(sArg) << endl;
			continue;
		}
		
		blobHeader bH;
		if(fread((void*)&bH, 1, sizeof(blobHeader), f) != sizeof(blobHeader))
		{
			cout << "Error reading number of resources in file " << ws2s(sArg) << endl;
			fclose(f);
			continue;
		}
		
		bPack[iPak] = false;
		
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
			lResourceHeaders[iPak].push_back(rH);
			
			if(mModResources.count(rH.id))
				bPack[iPak] = true;
		}
		
		if(bPack[iPak])	//Unpack this resource only if we need to
		{
			cout << "Unpacking resource " << ws2s(sArg) << endl;
			splitOutFiles(f, &lResourceHeaders[iPak], false);
		}
		
		fclose(f);
	}
	
	//Now we have all the .pak files we need extracted. Copy mod files over
	copyTempFiles();
	
	//And recompress
	for(int iPak = 0; iPak < 3; iPak++)
	{
		if(!bPack[iPak])
			continue;
		wstring sArg;
		
		switch(iPak)
		{
			case 0:
				sArg = RESOURCE_1_NAME;
				break;
			case 1:
				sArg = RESOURCE_2_NAME;
				break;
			default:
				sArg = RESOURCE_3_NAME;
				break;
		}
		
		cout << "Repacking resource blob file " << ws2s(sArg) << endl;
		
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
		bh.numItems = lResourceHeaders[iPak].size();
		
		fwrite(&bh, 1, sizeof(blobHeader), f);
		
		//Get the starting file pos for where we (should) be writing objects to
		size_t offsetPos = sizeof(blobHeader) + (lResourceHeaders[iPak].size() * sizeof(resourceHeader));	
		
		//Add the table of contents
		cout << "Adding table of contents..." << endl;
		for(list<resourceHeader>::iterator i = lResourceHeaders[iPak].begin(); i != lResourceHeaders[iPak].end(); i++)
		{
			resourceHeader rH = *i;
			
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", rH.id);
			
			rH.offset = offsetPos;	//Offset
			rH.size = ttvfs::GetFileSize(cIDFilename);	//Size of file
			if(mModHeader.count(rH.id))
				rH.flags = mModHeader[rH.id];	//Set the flags to the flags of this file copied over
			
			fwrite(&rH, 1, sizeof(resourceHeader), f);	//Write this to the file
			
			offsetPos += rH.size;	//Add this size to our running tally of where we are
		}
		
		//Add actual resource data
		cout << "Adding files..." << endl;
		for(list<resourceHeader>::iterator i = lResourceHeaders[iPak].begin(); i != lResourceHeaders[iPak].end(); i++)
		{			
			//Write this file
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", i->id);
			
			size_t fileSize = ttvfs::GetFileSize(cIDFilename);
			wchar_t* cTemp = (wchar_t*)malloc(fileSize);
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
	}
	
	removeTempFiles();
	
	//Done
	cout << endl << "Done." << endl;
	
	iTicks = GetTickCount() - iTicks;
	int iSeconds = iTicks / 1000;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	oWarnings << "Done." << endl;
	oWarnings.close();
	return 0;
}