#include "pakDataTypes.h"

list<ThreadConvertHelper> g_lThreadedResources;
u32 g_iCurResource;
u32 g_iNumResources;
HANDLE ghMutex;
bool g_bProgressOverwrite;
unsigned int g_iNumThreads;

DWORD WINAPI decompressResource(LPVOID lpParam)
{
	for(bool bDone = false;!bDone;)	//Loop until we're done
	{
		ThreadConvertHelper dh;
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
					g_lThreadedResources.pop_front();	//Done with this element
				}
				
				//Let user know which resource we're converting now
				if(!bDone)
				{
					if(g_bProgressOverwrite)
					{
						cout << "\rDecompressing file " << ++g_iCurResource << " out of " << g_iNumResources;
						cout.flush();
					}
					else
						cout << "Decompressing file " << ++g_iCurResource << " out of " << g_iNumResources << ": " << ws2s(dh.sFilename) << endl;
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
			
		if(dh.bCompressed)	//Compressed
		{
			uint8_t* tempData = decompress(&dh.data);
			if(tempData == NULL)
			{
				cout << "Error decompressing file " << ws2s(dh.sFilename) << endl;
				return 1;
			}
			free(dh.data.data);	//Free this compressed memory
			dh.data.data = tempData;	//Now we have the decompressed data
		}
		
		//See if this was a PNG image. Convert PNG images from the data in RAM
		if(dh.sFilename.find(TEXT(".png")) != wstring::npos ||
		   dh.sFilename.find(TEXT(".PNG")) != wstring::npos ||
		   dh.sFilename.find(TEXT("coloritemicon")) != wstring::npos ||
		   dh.sFilename.find(TEXT("colorbgicon")) != wstring::npos ||
		   dh.sFilename.find(TEXT("greybgicon")) != wstring::npos)			//Also would include .png.normal files as well
		{
			convertToPNG(dh.sFilename.c_str(), dh.data.data, dh.data.decompressedSize);	//Do the conversion to PNG
		}
		else	//For other file types, go ahead and write to the file before converting
		{
			//Write this out to the file
			FILE* fOut = _wfopen(dh.sFilename.c_str(), TEXT("wb"));
			if(fOut == NULL)
			{
				cout << "Unable to open output file " << ws2s(dh.sFilename) << endl;
				return 1;
			}
			fwrite(dh.data.data, 1, dh.data.decompressedSize, fOut);
			fclose(fOut);
		}
		free(dh.data.data);	//Free memory from this file
		
		//Convert wordPackDict.dat to XML
		if(dh.sFilename.find(TEXT("wordPackDict.dat")) != wstring::npos)
		{
			wordPackToXML(dh.sFilename.c_str());
			unlink(ws2s(dh.sFilename).c_str());
		}
		
		//Convert sndmanifest.dat to XML
		else if(dh.sFilename.find(TEXT("sndmanifest.dat")) != wstring::npos)
		{
			sndManifestToXML(dh.sFilename.c_str());
			unlink(ws2s(dh.sFilename).c_str());
		}
		
		//Convert itemmanifest.dat to XML
		else if(dh.sFilename.find(TEXT("itemmanifest.dat")) != wstring::npos)
		{
			itemManifestToXML(dh.sFilename.c_str());
			//TODO unlink(ws2s(dh.sFilename).c_str());
		}
		
		//Convert residmap.dat to XML
		else if(dh.sFilename.find(TEXT("residmap.dat")) != wstring::npos)
		{
			residMapToXML(dh.sFilename.c_str());
			unlink(ws2s(dh.sFilename).c_str());
		}
		
		//Convert .flac binary files to OGG
		else if(dh.sFilename.find(TEXT(".flac")) != wstring::npos ||
				dh.sFilename.find(TEXT(".FLAC")) != wstring::npos)
		{
			wstring s = dh.sFilename;
			s += TEXT(".ogg");
			binaryToOgg(dh.sFilename.c_str(), s.c_str());
			unlink(ws2s(dh.sFilename).c_str());	//Delete temporary .flac file
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







