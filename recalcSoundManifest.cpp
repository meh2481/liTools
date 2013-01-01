#include "pakDataTypes.h"

const wchar_t* cFilename = TEXT("vdata/sndmanifest.dat.xml");
list<wstring> g_lsOggFiles;
map<wstring, takeRecord> g_mtrTakes;
int g_iCurResource;
int g_iNumResourcesTotal;

HANDLE g_hInMutex;	//Mutex for g_lsOggFiles
HANDLE g_hOutMutex;	//Mutex for g_mtrTakes

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

DWORD WINAPI oggParse(LPVOID lpParam)
{
	for(bool bDone = false;!bDone;)	//Loop until we're done
	{
		wstring sFile;
		DWORD dwWaitResult = WaitForSingleObject(g_hInMutex,    // wait for mutex
												 INFINITE);  // no time-out interval
		
		switch (dwWaitResult) 
		{
			// The thread got ownership of the mutex
			case WAIT_OBJECT_0:
				if(!g_lsOggFiles.size())	//Done
					bDone = true;
				else
				{
					//Grab the top item off the list
					sFile = g_lsOggFiles.front();
					g_lsOggFiles.pop_front();	//Done with this element
				}
				
				//Let user know which resource we're converting now
				if(!bDone)
					cout << "Parsing sound " << ++g_iCurResource << " out of " << g_iNumResourcesTotal << ": " << ws2s(sFile) << endl;
				
				// Release ownership of the mutex object
				if (!ReleaseMutex(g_hInMutex)) 
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
		
		//Parse this OGG file
		takeRecord tr = getOggData(sFile.c_str());
		
		//Save this result
		dwWaitResult = WaitForSingleObject(g_hOutMutex, INFINITE);
		
		switch(dwWaitResult)
		{
			// The thread got ownership of the mutex
			case WAIT_OBJECT_0:
				g_mtrTakes[sFile] = tr;	//Store this
				
				// Release ownership of the mutex object
				if (!ReleaseMutex(g_hOutMutex)) 
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
	}
	return 0;
}

bool threadedOgg()
{
	g_iCurResource = 0;
	g_iNumResourcesTotal = g_lsOggFiles.size();
	
	//Create mutex
	g_hInMutex = CreateMutex(NULL, FALSE, NULL);
	g_hOutMutex = CreateMutex(NULL, FALSE, NULL);

    if(g_hInMutex == NULL || g_hOutMutex == NULL) 
    {
        cout << "Error: Unable to create mutexes for multithreaded OGG parsing. Aborting..." << endl;
        return false;
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
                     (LPTHREAD_START_ROUTINE) oggParse, 
                     NULL,       // no thread function arguments
                     0,          // default creation flags
                     NULL); 	 // no thread identifier

        if( aThread[i] == NULL )
        {
            cout << "CreateThread error: " << GetLastError() << endl;
			free(aThread);
            return false;
        }
    }

    // Wait for all threads to terminate
    WaitForMultipleObjects(iNumThreads, aThread, TRUE, INFINITE);

    // Close thread and mutex handles
    for(u32 i = 0; i < iNumThreads; i++ )
        CloseHandle(aThread[i]);

    CloseHandle(g_hInMutex);
	CloseHandle(g_hOutMutex);
	
	free(aThread);
	return true;
}

int main(int argc, char** argv)
{
	DWORD iTicks = GetTickCount();
	
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(ws2s(cFilename).c_str());
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << cFilename << ": Error " << iErr << endl;
		delete doc;
		return 1;
	}
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << cFilename << endl;
		delete doc;
		return 1;
	}
	int iTotalSounds = 0;
	root->QueryIntAttribute("numtakes", &iTotalSounds);
	
	cout << endl << "Parsing XML..." << endl;
	//Get the filenames that we'll be parsing
	XMLElement* elem = root->FirstChildElement("sound");
	while(elem != NULL)
	{
		//Get the takes for this sound
		XMLElement* elem2 = elem->FirstChildElement("take");
		while(elem2 != NULL)
		{
			const char* cName = elem2->Attribute("filename");
			if(cName == NULL)
			{
				cout << "Error: Unable to get filename of take record in file " << cFilename << endl;
				delete doc;
				return 1;
			}
			g_lsOggFiles.push_back(s2ws(cName));	//Save this for later
			
			elem2 = elem2->NextSiblingElement("take");	//Next item
		}
		elem = elem->NextSiblingElement("sound");	//Next item
	}
	
	//Start the threads running that'll get our OGG data
	if(!threadedOgg())
	{
		cout << "Abort." << endl;
		return 1;
	}
	
	//Roll through child elements
	cout << endl << "Filling XML in with vorbis data..." << endl;
	elem = root->FirstChildElement("sound");
	//int iCurSound = 0;
	while(elem != NULL)
	{
		//Get the takes for this sound
		XMLElement* elem2 = elem->FirstChildElement("take");
		while(elem2 != NULL)
		{
			const char* cName = elem2->Attribute("filename");	//Assume this works, since we've tested for this == NULL already
			//cout << "Parsing sound " << ++iCurSound << " out of " << iTotalSounds << ": " << cName << endl;
			takeRecord tr = g_mtrTakes[s2ws(cName)];	//Grab the header data that we've parsed
			if(tr.resId != 0)
			{
				//Update this info
				elem2->SetAttribute("channels", tr.channels);
				elem2->SetAttribute("samplespersec", tr.samplesPerSec);
				elem2->SetAttribute("samplecountperchannel", tr.sampleCountPerChannel);
				elem2->SetAttribute("vorbisworkingsetsizebytes", tr.vorbisWorkingSetSizeBytes);
				elem2->SetAttribute("vorbismarkerssizebytes", tr.vorbisMarkersSizeBytes);
				elem2->SetAttribute("vorbispacketssizebytes", tr.vorbisPacketsSizeBytes);
			}
			//else
			//	cout << "Could not open file " << cName << "; Skipping" << endl;
			
			elem2 = elem2->NextSiblingElement("take");	//Next item
		}
		elem = elem->NextSiblingElement("sound");	//Next item
	}
	
	doc->SaveFile(ws2s(cFilename).c_str());	//Save this back
	delete doc;	//We're done with this
	
	cout << "Done" << endl;
	
	iTicks = GetTickCount() - iTicks;
	int iSeconds = iTicks / 1000;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	return 0;
}


