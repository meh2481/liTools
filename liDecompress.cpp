#include "pakDataTypes.h"

extern list<decompressHelper> g_lThreadedResources;
extern bool g_bProgressOverwrite;
extern unsigned int g_iNumThreads;

ttvfs::VFSHelper vfs;

//Remove all files in the temp/ folder, since we're done with them
/*void removeTempFiles()
{
	ttvfs::StringList slFiles;
    ttvfs::GetFileList("temp", slFiles);

    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    {
		string s = ("temp/") + (*il);
		unlink(s.c_str());	//Remove this file
    }
	rmdir("temp/");	//Remove the folder itself
}*/

/*bool CreateDir(const char *dir)
{
    if(ttvfs::IsDirectory(dir)) // do not try to create if it already exists
        return true;
    bool result;
# if _WIN32
    result = !!::CreateDirectory(s2ws(dir).c_str(), NULL);
# else
    result = !mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    return result;
}

bool CreateDirRec(const char *dir)
{
    if(ttvfs::IsDirectory(dir))
        return true;
    bool result = true;
    ttvfs::StringList li;
    ttvfs::StrSplit(dir, "/\\", li, false);
    std::string d;
    d.reserve(strlen(dir) + 1);
    if(*dir == '/')
        d += '/';
    bool last = false;
    for(ttvfs::StringList::iterator it = li.begin(); it != li.end(); ++it)
    {
        d += *it;
        last = CreateDir(d.c_str());
        result = last && result;
        d += '/';
    }
    return result || last;
}*/

//Create the folder that this resource ID's file will be placed in
void makeFolder(u32 resId)
{
	const char* cName = ws2s(getName(resId)).c_str();
	
	for(int i = strlen(cName)-1; i >= 0; i--)
	{
		if(cName[i] == '/')
		{
			char* cFilename = (char*)malloc(i+2);
			memset(cFilename, '\0', i+2);
			strncpy(cFilename, cName, i+1);
			//wchar_t cData[512];
			//sprintf(cData, "output/%s", cFilename);
			//if(!ttvfs::IsDirectory(cFilename))
			ttvfs::CreateDirRec(cFilename);
			free(cFilename);
		}
	}
}

void parseCmdLine(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		if(s == "--overwrite-progress")
			g_bProgressOverwrite = true;
		else if(s.find("--threads=") != string::npos)
		{
			size_t pos = s.find('=')+1;
			if(s.length() <= pos)
			{
				cout << "missing thread count" << endl;
				continue;
			}
			int iNumThreads = atoi(&s.c_str()[pos]);
			if(iNumThreads < 0 ||
			   iNumThreads > MAX_NUM_THREADS)
			{
				cout << "Invalid number of threads: " << iNumThreads << endl;
				continue;
			}
			g_iNumThreads = iNumThreads;
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
	DWORD iTicks = GetTickCount();	//Store the starting number of milliseconds
	
	vfs.Prepare();
		
	//read in the resource names to unpack
	initResMap();
	initSoundManifest();
	parseCmdLine(argc,argv);
	
	if(argc < 2)
	{
		cout << "Usage: liDecompress [filename1] [filename2] ... [filenameN]" << endl;
		//system("PAUSE");
		return 0;
	}
	
	for(int iArg = 1; iArg < argc; iArg++)
	{
		if(argv[iArg][0] == '-')	//Skip over commandline switches
			continue;
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
		/*removeTempFiles();
		if(!ttvfs::IsDirectory("temp"))
			ttvfs::CreateDirRec("temp");*/
			
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
			decompressHelper dh;
			//tch.bCompressed = false;
			makeFolder(i->id);
			const wchar_t* cName = getName(i->id);
			oPakList << ws2s(cName) << endl;
			fseek(f, i->offset, SEEK_SET);
			dh.sFilename = cName;
			if(i->flags == FLAG_ZLIBCOMPRESSED)
			{
				compressedHeader cH;
				if(fread((void*)&cH, 1, sizeof(compressedHeader), f) != sizeof(compressedHeader))
				{
					cout << "Error reading compressed header." << endl;
					fclose(f);
					continue;
				}
				
				//char sOutFile[256];
				//sprintf(sOutFile, "temp/%u", i->id);
				//FILE* fOut = fopen(sOutFile, "wb");
				
				uint32_t size = cH.compressedSizeBytes;
				
				uint8_t* buf = (uint8_t*)malloc(size);
				size_t sizeRead = fread((void*)buf, 1, size, f);
				if(sizeRead != size)
				{
					cout << "Error reading compressed data. Size: " << size << " read: " << sizeRead << endl;
					fclose(f);
					//fclose(fOut);
					free(buf);
					continue;
				}
				dh.data.data = buf;
				dh.data.compressedSize = cH.compressedSizeBytes;
				dh.data.decompressedSize = cH.uncompressedSizeBytes;
				dh.bCompressed = true;
				//fwrite((void*)buf, 1, size, fOut);
				//fclose(fOut);
				//free(buf);
				
				//tch.sIn = s2ws(sOutFile);
				//tch.bCompressed = true;
			}
			else if(i->flags == FLAG_NOCOMPRESSION)
			{
				//tch.sIn = TEXT("");
				//FILE* fOut = _wfopen(cName, TEXT("wb"));
				uint8_t* buf = (uint8_t*)malloc(i->size);
			  
				if(fread((void*)buf, 1, i->size, f) != i->size)
				{
					cout << "Error reading non-compressed data." << endl;
					fclose(f);
					//fclose(fOut);
					free(buf);
					continue;
				}
				dh.data.data = buf;
				dh.data.compressedSize = dh.data.decompressedSize = i->size;
				dh.bCompressed = false;
				//fwrite((void*)buf, 1, i->size, fOut);
				
			  
				//free(buf);
				//fclose(fOut);
			}
			else
				cout << "Invalid resource flag " << i->flags << endl;
			
			g_lThreadedResources.push_back(dh);
			
		}
		
		threadedDecompress();
		
		fclose(f);
		oPakList.close();
	}
	
	//removeTempFiles();
	cout << "\rDone.                                " << endl;
	
	iTicks = GetTickCount() - iTicks;
	float iSeconds = (float)iTicks / 1000.0;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	//system("PAUSE");
	
	return 0;
}
