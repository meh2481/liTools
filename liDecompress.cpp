#include "pakDataTypes.h"

extern list<ThreadConvertHelper> g_lThreadedResources;
extern bool g_bProgressOverwrite;
extern unsigned int g_iNumThreads;

ttvfs::VFSHelper vfs;

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
		
		//Iterate through these items, splitting them out of the file and creating new files out of each
		cout << "Extracting files..." << endl;
		for(list<resourceHeader>::iterator i = lResourceHeaders.begin(); i != lResourceHeaders.end(); i++)
		{
			ThreadConvertHelper dh;
			//makeFolder(i->id);
			//const wchar_t* cName = getName(i->id);
			//oPakList << ws2s(cName) << endl;
			fseek(f, i->offset, SEEK_SET);
			//dh.sFilename = cName;
			dh.id = i->id;
			if(i->flags & FLAG_ZLIBCOMPRESSED)
			{
				compressedHeader cH;
				if(fread((void*)&cH, 1, sizeof(compressedHeader), f) != sizeof(compressedHeader))
				{
					cout << "Error reading compressed header." << endl;
					fclose(f);
					continue;
				}
				
				uint32_t size = cH.compressedSizeBytes;
				
				uint8_t* buf = (uint8_t*)malloc(size);
				size_t sizeRead = fread((void*)buf, 1, size, f);
				if(sizeRead != size)
				{
					cout << "Error reading compressed data. Size: " << size << " read: " << sizeRead << endl;
					fclose(f);
					free(buf);
					continue;
				}
				dh.data.data = buf;
				dh.data.compressedSize = cH.compressedSizeBytes;
				dh.data.decompressedSize = cH.uncompressedSizeBytes;
				dh.bCompressed = true;
			}
			else// if(i->flags & FLAG_ZLIBCOMPRESSED == FLAG_NOCOMPRESSION)	//Uncompressed
			{
				uint8_t* buf = (uint8_t*)malloc(i->size);
			  
				if(fread((void*)buf, 1, i->size, f) != i->size)
				{
					cout << "Error reading non-compressed data." << endl;
					fclose(f);
					free(buf);
					continue;
				}
				dh.data.data = buf;
				dh.data.compressedSize = dh.data.decompressedSize = i->size;
				dh.bCompressed = false;
			}
			/*else
			{
				cout << "Invalid resource flag " << i->flags << ". Skipping resource " << i->id << endl;
				continue;
			}*/
			
			g_lThreadedResources.push_back(dh);
			
		}
		
		threadedDecompress();
		
		fclose(f);
		ofstream oPakList(sPakListFilename.c_str());
		wstring sIsResidFilename = getName(lResourceHeaders.front().id);
		if(sIsResidFilename == TEXT(RESIDMAP_NAME))
			lResourceHeaders.pop_front();	//HACK: So we don't end up with recursive residmap.dat files in our pakfiles...
		for(list<resourceHeader>::iterator i = lResourceHeaders.begin(); i != lResourceHeaders.end(); i++)
		{
			oPakList << ws2s(getName(i->id)) << endl;
		}
		oPakList.close();
	}
	cout << "\rDone.                                " << endl;
	
	iTicks = GetTickCount() - iTicks;
	float iSeconds = (float)iTicks / 1000.0;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	//system("PAUSE");
	
	return 0;
}
