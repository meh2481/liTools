#include "pakDataTypes.h"

list<ThreadConvertHelper> g_lThreadedResources;
u32 g_iCurResource;
u32 g_iNumResources;
HANDLE ghMutex;
bool g_bProgressOverwrite;
unsigned int g_iNumThreads;

//Create the folder that this resource ID's file will be placed in
void makeFolder(u32 resId)
{
	wstring sFilename = getName(resId);
	size_t pos = sFilename.find_last_of(L'/');
	if(pos != wstring::npos)
		sFilename = sFilename.substr(0,pos);
	sFilename = TEXT("./") + sFilename;
	//cout << "Creating folder " << ws2s(sFilename) << endl;
	ttvfs::CreateDirRec(ws2s(sFilename).c_str());
}

DWORD WINAPI decompressResource(LPVOID lpParam)
{
	for(bool bDone = false;!bDone;)	//Loop until we're done
	{
		ThreadConvertHelper dh;
		wstring sFilename;
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
					dh = g_lThreadedResources.front();
					sFilename = getName(dh.id);	//Mutex on this too, since getName() isn't really threadsafe
					makeFolder(dh.id);	//Also create folder (not threadsafe, either)
					g_lThreadedResources.pop_front();	//Done with this element
				}
				
				//Let user know which resource we're converting now
				if(!bDone)
				{
					g_iCurResource++;
					if(!(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1))
					{
						if(g_bProgressOverwrite)
						{
							cout << "\rDecompressing file " << g_iCurResource << " out of " << g_iNumResources;
							cout.flush();
						}
						else
							cout << "Decompressing file " << g_iCurResource << " out of " << g_iNumResources << ": " << ws2s(sFilename) << endl;
					}
				}
				
				// Release ownership of the mutex object
				if(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1)	//Don't release residmap.dat mutex until we've read in all the filenames
				{
					g_iNumResources--;
				}
				else
				{
					if (!ReleaseMutex(ghMutex)) 
					{ 
						cout << "Error: Unable to release mutex." << endl;
						return 1;
					}
				}
				break; 

			// The thread got ownership of an abandoned mutex
			// This is an indeterminate state
			case WAIT_ABANDONED: 
				cout << "Error: Abandoned mutex" << endl;
				return 1; 
		}
		if(bDone)
		{
			if(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1)
				ReleaseMutex(ghMutex);
			continue;	//Stop here if done
		}
			
		if(dh.bCompressed)	//Compressed
		{
			uint8_t* tempData = decompress(&dh.data);
			if(tempData == NULL)
			{
				cout << "Error decompressing file " << ws2s(sFilename) << endl;
				if(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1)
					ReleaseMutex(ghMutex);
				return 1;
			}
			free(dh.data.data);	//Free this compressed memory
			dh.data.data = tempData;	//Now we have the decompressed data
		}
		
		//See if this was a PNG image. Convert PNG images from the data in RAM
		if(sFilename.find(TEXT(".png")) != wstring::npos ||
		   sFilename.find(TEXT(".PNG")) != wstring::npos ||
		   sFilename.find(TEXT("coloritemicon")) != wstring::npos ||
		   sFilename.find(TEXT("colorbgicon")) != wstring::npos ||
		   sFilename.find(TEXT("greybgicon")) != wstring::npos)			//Also would include .png.normal files as well
		{
			convertToPNG(sFilename.c_str(), dh.data.data, dh.data.decompressedSize);	//Do the conversion to PNG
		}
		else	//For other file types, go ahead and write to the file before converting
		{
			//Write this out to the file
			FILE* fOut = _wfopen(sFilename.c_str(), TEXT("wb"));
			if(fOut == NULL)
			{
				cout << "Unable to open output file " << ws2s(sFilename) << endl;
				if(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1)
					ReleaseMutex(ghMutex);
				return 1;
			}
			fwrite(dh.data.data, 1, dh.data.decompressedSize, fOut);
			fclose(fOut);
		}
		free(dh.data.data);	//Free memory from this file
		
		//Convert wordPackDict.dat to XML
		if(sFilename.find(TEXT("wordPackDict.dat")) != wstring::npos)
		{
			wordPackToXML(sFilename.c_str());
			unlink(ws2s(sFilename).c_str());
		}
		
		//Convert sndmanifest.dat to XML
		else if(sFilename.find(TEXT("sndmanifest.dat")) != wstring::npos)
		{
			sndManifestToXML(sFilename.c_str());
			//unlink(ws2s(sFilename).c_str());
		}
		
		//Convert itemmanifest.dat to XML
		else if(sFilename.find(TEXT("itemmanifest.dat")) != wstring::npos)
		{
			itemManifestToXML(sFilename.c_str());
			//TODO unlink(ws2s(sFilename).c_str());
		}
		
		//Convert residmap.dat to XML
		else if(sFilename.find(TEXT("residmap.dat")) != wstring::npos)
		{
			residMapToXML(sFilename.c_str());
			unlink(ws2s(sFilename).c_str());
		}
		
		//Convert .flac binary files to OGG
		else if(sFilename.find(TEXT(".flac")) != wstring::npos ||
				sFilename.find(TEXT(".FLAC")) != wstring::npos)
		{
			wstring s = sFilename;
			s += TEXT(".ogg");
			binaryToOgg(sFilename.c_str(), s.c_str());
			unlink(ws2s(sFilename).c_str());	//Delete temporary .flac file
		}
		
		//Convert vdata/fontmanifest.dat to XML
		else if(sFilename.find(TEXT("fontmanifest.dat")) != wstring::npos)
		{
			fontManifestToXML(sFilename);
			unlink(ws2s(sFilename).c_str());
		}
		
		//Convert font files to XML
		else if(sFilename.find(TEXT(".font.xml")) != wstring::npos)
		{
			fontToXML(sFilename);
		}
		
		if(sFilename == TEXT(RESIDMAP_NAME) && g_iCurResource == 1)
		{
			ReleaseMutex(ghMutex);
			g_iCurResource--;
		}
	}
	return 0;
}

void threadedDecompress()
{
	g_iCurResource = 0;
	g_iNumResources = g_lThreadedResources.size();
	
	//Create mutex
	ghMutex = CreateMutex(NULL,              // default security attributes
						  FALSE,             // initially not owned
					      NULL);             // unnamed mutex

    if (ghMutex == NULL) 
    {
        cout << "Error: Unable to create mutex for multithreaded decompression. Aborting..." << endl;
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
                     (LPTHREAD_START_ROUTINE) decompressResource, 
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







