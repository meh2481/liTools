//~ 3:08 when single-threaded
//~ 1:43 when multi-threaded

#include "pakDataTypes.h"
#include "png.h"
#include <VFS.h>
#include <VFSTools.h>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
using namespace std;

list<ThreadConvertHelper> g_lThreadedResources;
u32 iCurResource;
u32 iNumResources;
HANDLE ghMutex;

DWORD WINAPI decompressResource(LPVOID lpParam)
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
					cout << "Converting resource " << ++iCurResource << " out of " << iNumResources << ": " << tch.sFilename << endl;
				
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
			
		if(tch.bCompressed)
			compdecomp(tch.sIn.c_str(), tch.sFilename.c_str());
		
		const char* cName = tch.sFilename.c_str();
			
		//See if this was a PNG image
		if(strstr(cName, ".png") != NULL ||
		   strstr(cName, ".PNG") != NULL ||
		   strstr(cName, "coloritemicon") != NULL ||
		   strstr(cName, "colorbgicon") != NULL ||
		   strstr(cName, "greybgicon") != NULL)			//Also would include .png.normal files as well
		{
			convertPNG(cName);	//Do the conversion
		}
		
		//Convert .flac binary files to OGG
		if(strstr(cName, ".flac") != NULL ||
		   strstr(cName, ".FLAC") != NULL)
		{
			string s = cName;
			s += ".ogg";
			binaryToOgg(cName, s.c_str());
			unlink(cName);	//Delete temporary .flac file
		}
	}
	return 0;
}

void threadedDecompress()
{
	iCurResource = 0;
	iNumResources = g_lThreadedResources.size();
	
	//Create mutex
	ghMutex = CreateMutex(NULL,              // default security attributes
						  FALSE,             // initially not owned
					      NULL);             // unnamed mutex

    if (ghMutex == NULL) 
    {
        cout << "ERROR: Unable to create mutex for multithreded decompression. Aborting..." << endl;
        return;
    }
	
	//Get how many processor cores we have, so we know how many threads to create
	SYSTEM_INFO siSysInfo; 
    GetSystemInfo(&siSysInfo);
	
	u32 iNumThreads = siSysInfo.dwNumberOfProcessors;
	
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
            cout << "CreateThread error: %d\n" << GetLastError() << endl;
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







