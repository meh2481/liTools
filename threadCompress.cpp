#include "pakDataTypes.h"

list<ThreadConvertHelper> g_lThreadedResources;
extern map<wstring, pakHelper> g_pakHelping;	//in liCompress.cpp, used for packing stuff into the .pak files
u32 g_iCurResource;
u32 g_iNumResources;
HANDLE ghMutex;
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
		ThreadConvertHelper tch;
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
						cout << "Compressing file " << ++g_iCurResource << " out of " << g_iNumResources << ": " << ws2s(tch.sIn) << endl;
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
		
		bool bNormalConvert = false;
		wstring sDeleteWhenDone = TEXT("");
			
		if(tch.sIn.find(TEXT(".flac")) != wstring::npos ||
		   tch.sIn.find(TEXT(".FLAC")) != wstring::npos)
		{
			wstring s = tch.sIn + TEXT(".ogg");
			oggToBinary(s.c_str(), tch.sFilename.c_str());
			WaitForSingleObject(ghMutex, INFINITE);
			g_pakHelping[tch.sIn].bCompressed = false;	//No compression for OGG streams, since these are compressed already
			ReleaseMutex(ghMutex);
		}
		//If this was a PNG image
		else if(tch.sIn.find(TEXT(".png")) != wstring::npos ||
			    tch.sIn.find(TEXT(".PNG")) != wstring::npos ||
			    tch.sIn.find(TEXT("coloritemicon")) != wstring::npos ||
			    tch.sIn.find(TEXT("colorbgicon")) != wstring::npos ||
			    tch.sIn.find(TEXT("greybgicon")) != wstring::npos)			//Also would include .png.normal files as well
		{
			convertFromPNG(tch.sIn.c_str());	//Do the conversion
			
			wstring s = tch.sIn + TEXT(".temp");	//Use the decompressed PNG for this
			cout << "Compress result: " << compdecomp(s.c_str(), tch.sFilename.c_str(), 1) << endl;
			WaitForSingleObject(ghMutex, INFINITE);
			g_pakHelping[tch.sIn].bCompressed = true;
			g_pakHelping[tch.sIn].cH.uncompressedSizeBytes = getFileSize(s.c_str());	//Hang onto these for compressed header stuff
			g_pakHelping[tch.sIn].cH.compressedSizeBytes = getFileSize(tch.sFilename.c_str());
			ReleaseMutex(ghMutex);
			unlink(ws2s(s).c_str());	//Remove the temporary file
		}
		else if(tch.sIn.find(TEXT("wordPackDict.dat")) != wstring::npos)
		{
			XMLToWordPack(tch.sIn.c_str());	//De-XML this first
			bNormalConvert = true;	//Behave like normal
			sDeleteWhenDone = tch.sIn;
		}
		else if(tch.sIn.find(TEXT("sndmanifest.dat")) != wstring::npos)
		{
			XMLToSndManifest(tch.sIn.c_str());
			bNormalConvert = true;
			sDeleteWhenDone = tch.sIn;
		}
		else if(tch.sIn.find(TEXT("itemmanifest.dat")) != wstring::npos)
		{
			XMLToItemManifest(tch.sIn.c_str());
			bNormalConvert = true;
			//TODO sDeleteWhenDone = tch.sIn;
		}
		else if(tch.sIn.find(TEXT("residmap.dat")) != wstring::npos)
		{
			XMLToResidMap(tch.sIn.c_str());
			bNormalConvert = true;
			sDeleteWhenDone = tch.sIn;
		}
		else
			bNormalConvert = true;
		
		if(bNormalConvert)
		{
			if(compdecomp(tch.sIn.c_str(), tch.sFilename.c_str(), 1))
			{
				cout << "Error: Unable to compress file " << ws2s(tch.sIn) << ". Abort." << endl;
				exit(1);
			}
			WaitForSingleObject(ghMutex, INFINITE);
			g_pakHelping[tch.sIn].bCompressed = true;
			g_pakHelping[tch.sIn].cH.uncompressedSizeBytes = getFileSize(tch.sIn.c_str());	//Hang onto these for compressed header stuff
			g_pakHelping[tch.sIn].cH.compressedSizeBytes = getFileSize(tch.sFilename.c_str());
			ReleaseMutex(ghMutex);
		}
		
		//If we wish to delete this file when we're done
		if(sDeleteWhenDone != TEXT(""))
			unlink(ws2s(sDeleteWhenDone).c_str());
	}
	return 0;
}

void threadedCompress()
{
	g_iCurResource = 0;
	g_iNumResources = g_lThreadedResources.size();
	
	//Create mutex
	ghMutex = CreateMutex(NULL,              // default security attributes
						  FALSE,             // initially not owned
					      NULL);             // unnamed mutex

    if (ghMutex == NULL) 
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
	
	free(aThread);
}







