#include "pakDataTypes.h"

list<wstring> g_lThreadedResources;
extern map<wstring, pakHelper> g_pakHelping;	//in liCompress.cpp, used for packing stuff into the .pak files
u32 g_iCurResource;
u32 g_iNumResources;
HANDLE ghMutex;
HANDLE ghOutMutex;
bool g_bProgressOverwrite;
unsigned int g_iNumThreads;

i32 getFileSize(const wchar_t* cFilename)	//Since TTVFS isn't totally threadsafe, we need some way of determining file size
{
	FILE *f = _wfopen(cFilename, TEXT("rb"));
	fseek(f, 0, SEEK_END);
    i32 iSz = ftell(f);
    fclose(f);
	return iSz;
}

DWORD WINAPI compressResource(LPVOID lpParam)
{
	for(bool bDone = false;!bDone;)	//Loop until we're done
	{
		wstring tch;
		DWORD dwWaitResult = WaitForSingleObject(ghMutex,    // wait for mutex
												 INFINITE);  // no time-out interval
		
		switch (dwWaitResult) 
		{
			// The thread got ownership of the mutex
			case WAIT_OBJECT_0:
				if(!g_lThreadedResources.size())	//Done
					bDone = true;
				else
				{
					//Grab the top item off the list
					tch = g_lThreadedResources.front();
					g_lThreadedResources.pop_front();	//Done with this element
				}
				
				//Let user know which resource we're converting now
				if(!bDone)
				{
					if(g_bProgressOverwrite)
					{
						cout << "\rCompressing file " << ++g_iCurResource << " out of " << g_iNumResources;
						cout.flush();
					}
					else
						cout << "Compressing file " << ++g_iCurResource << " out of " << g_iNumResources << ": " << ws2s(tch) << endl;
				}
				
				// Release ownership of the mutex object
				if (!ReleaseMutex(ghMutex)) 
				{ 
					cout << "Error: Unable to release mutex." << endl;
					return 1;
				}
				break; 

			// The thread got ownership of an abandoned mutex
			// This is an indeterminate state
			case WAIT_ABANDONED: 
				cout << "Error: Abandoned mutex" << endl;
				return 1; 
		}
		if(bDone)
			continue;	//Stop here if done
		
		wstring sDeleteWhenDone = TEXT("");	//If we have leftover files we want to delete when compression is done
		wstring sFileToPak = tch;			//The file we'll be sticking in the pakfile
		pakHelper ph;
		ph.cH.uncompressedSizeBytes = ph.cH.compressedSizeBytes = 0;	//Supress warning
		ph.bCompressed = true;				//Files are compressed by default
		
		//OGG sound
		if(tch.find(TEXT(".flac")) != wstring::npos ||
		   tch.find(TEXT(".FLAC")) != wstring::npos)
		{
			wstring s = tch + TEXT(".ogg");
			oggToBinary(s.c_str(), tch.c_str());
			sDeleteWhenDone = tch;
			//ph.bCompressed = false;	//No compression for OGG streams, since these are compressed already
		}
		//PNG image
		else if(tch.find(TEXT(".png")) != wstring::npos ||
			    tch.find(TEXT(".PNG")) != wstring::npos ||
			    tch.find(TEXT("coloritemicon")) != wstring::npos ||
			    tch.find(TEXT("colorbgicon")) != wstring::npos ||
			    tch.find(TEXT("greybgicon")) != wstring::npos)			//Also would include .png.normal files as well
		{
			convertFromPNG(tch.c_str());	//Do the conversion
			sDeleteWhenDone = sFileToPak = tch + TEXT(".temp");	//Use the unraveled PNG for this
		}
		else if(tch.find(TEXT("wordPackDict.dat")) != wstring::npos)
		{
			XMLToWordPack(tch.c_str());	//De-XML this first
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("sndmanifest.dat")) != wstring::npos)
		{
			XMLToSndManifest(tch.c_str());
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("itemmanifest.dat")) != wstring::npos)
		{
			XMLToItemManifest(tch.c_str());
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("combodb.dat")) != wstring::npos)
		{
			XMLToComboDB(tch.c_str());
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("residmap.dat")) != wstring::npos)
		{
			XMLToResidMap(tch.c_str());
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("fontmanifest.dat")) != wstring::npos)
		{
			XMLToFontManifest(tch);
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("letterdb.dat")) != wstring::npos)
		{
			XMLToLetter(tch);
			//TODO sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("catalogdb.dat")) != wstring::npos)
		{
			XMLToCatalog(tch);
			//TODO sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT(".font.xml")) != wstring::npos)
		{
			XMLToFont(tch);
			sFileToPak += TEXT(".temp");
			sDeleteWhenDone = sFileToPak;
		}
		else if(tch.find(TEXT("loctexmanifest.bin")) != wstring::npos)
		{
			XMLToLoctexManifest(tch);
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("myPicturesImage.dat")) != wstring::npos)
		{
			XMLToMyPictures(tch);
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("smokeImage.dat")) != wstring::npos)
		{
			XMLToSmokeImage(tch);
			sDeleteWhenDone = tch;
		}
		else if(tch.find(TEXT("fluidPalettes.dat")) != wstring::npos)
		{
			XMLToFluidPalettes(tch);
			sDeleteWhenDone = tch;
		}
		
		//Pull in the data from the file
		ph.dataSz = getFileSize(sFileToPak.c_str());
		ph.data = (uint8_t*)malloc(ph.dataSz);	//Allocate memory to hold the file data
		FILE* f = _wfopen(sFileToPak.c_str(), TEXT("rb"));	//Whatever file we're supposed to use
		if(f == NULL)
		{
			cout << "Error: Unable to open file " << ws2s(sFileToPak) << ". Abort." << endl;
			exit(1);
		}
		if(fread(ph.data, 1, ph.dataSz, f) != ph.dataSz)
		{
			cout << "Error reading from file " << ws2s(sFileToPak) << ". Abort." << endl;
			exit(1);
		}
		fclose(f);
		
		zlibData zdt;
		zdt.data = ph.data;
		zdt.compressedSize = zdt.decompressedSize = ph.dataSz;
		uint8_t* temp = compress(&zdt);
		if(temp == NULL)
		{
			ph.bCompressed = false;
		}
		else
		{
			ph.cH.uncompressedSizeBytes = zdt.decompressedSize;	//Hang onto these for compressed header stuff
			ph.cH.compressedSizeBytes = zdt.compressedSize;
			free(zdt.data);	//Free this uncompressed memory
			ph.data = temp;	//Hang onto the compressed memory
			ph.dataSz = zdt.compressedSize;	//And the compressed memory size
		}
		WaitForSingleObject(ghOutMutex, INFINITE);
		g_pakHelping[tch] = ph;	//Save this
		ReleaseMutex(ghOutMutex);
		
		//If we wish to delete a file when we're done
		if(sDeleteWhenDone != TEXT(""))
			unlink(ws2s(sDeleteWhenDone).c_str());
	}
	return 0;
}

void threadedCompress(list<wstring> resources)
{
	g_lThreadedResources = resources;
	g_iCurResource = 0;
	g_iNumResources = g_lThreadedResources.size();
	
	//Create mutexes
	ghMutex = CreateMutex(NULL,              // default security attributes
						  FALSE,             // initially not owned
					      NULL);             // unnamed mutex
	ghOutMutex = CreateMutex(NULL, FALSE, NULL);

    if (ghMutex == NULL || ghOutMutex == NULL) 
    {
        cout << "Error: Unable to create mutex for multithreaded compression. Aborting..." << endl;
        return;
    }
	
	//Get how many processor cores we have, so we know how many threads to create
	SYSTEM_INFO siSysInfo; 
    GetSystemInfo(&siSysInfo);
	
	u32 iNumThreads = siSysInfo.dwNumberOfProcessors;
	if(g_iNumThreads != 0)
		iNumThreads = g_iNumThreads;
	
	//Create memory for the threads
	HANDLE* aThread = (HANDLE*)malloc(sizeof(HANDLE) * iNumThreads);
	
	//Start threads
	for(u32 i = 0; i < iNumThreads; i++ )
    {
        aThread[i] = CreateThread( 
                     NULL,       // default security attributes
                     0,          // default stack size
                     (LPTHREAD_START_ROUTINE) compressResource, 
                     NULL,       // no thread function arguments
                     0,          // default creation flags
                     NULL); 	 // no thread identifier

        if( aThread[i] == NULL )
        {
            cout << "CreateThread error: " << GetLastError() << endl;
			free(aThread);
            return;
        }
    }

    // Wait for all threads to terminate
    WaitForMultipleObjects(iNumThreads, aThread, TRUE, INFINITE);

    // Close thread and mutex handles
    for(u32 i = 0; i < iNumThreads; i++ )
        CloseHandle(aThread[i]);

    CloseHandle(ghMutex);
	CloseHandle(ghOutMutex);
	
	free(aThread);
}







