#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

#define RESOURCE_1_NAME	"resource.pak"
#define RESOURCE_2_NAME	"embed.pak"
#define RESOURCE_3_NAME	"frontend.pak"

#define RESOURCE_TYPE	20480

#define RESOURCE_1	1
#define RESOURCE_2	2
#define RESOURCE_3	3

int pullPakFiles(int argc, char** argv)
{
	HMODULE hExe;        // handle to .EXE file
	
	if(argc != 2)
	{
		cout << "Usage: pullpakfiles.exe [little inferno executable filename]" << endl;
		return 1;
	}

	// Load the .EXE whose resources you want to list.
	//cout << "Loading libraries from executable..." << endl;
	//cout << "ticks before: " << GetTickCount() << endl;
	hExe = LoadLibrary(TEXT(argv[1]));
	//cout << "ticks after: " << GetTickCount() << endl;
	if (hExe == NULL)
	{
		cout << "Unable to load " << argv[1] << endl;
		return 1;
	}
	//cout << "50 percent done" << endl;
	
	//Grab resources
	for(int iResource = RESOURCE_1; iResource <= RESOURCE_3; iResource++)
	{
		HRSRC hResource = FindResource(hExe, MAKEINTRESOURCE(iResource), MAKEINTRESOURCE(RESOURCE_TYPE));
		if (!hResource) 
		{
			cout << "Error: Unable to find resource " << iResource << " in executable" << endl;
			return 1;
		}
		
		DWORD resLen = SizeofResource(hExe, hResource);
		if (!resLen)
		{
			cout << "Error: size of resource = 0" << endl;
			return 1;
		}
		
		if(resLen == 4)	//4 bytes means this resource has been pulled already
		{
			cout << "This executable has already been stripped. Aborting." << endl;
			return 1;
		}
		
		char cName[512];
		switch(iResource)
		{
			case RESOURCE_1:
				sprintf(cName, "%s", RESOURCE_1_NAME);
				break;
			
			case RESOURCE_2:
				sprintf(cName, "%s", RESOURCE_2_NAME);
				break;
				
			default:
				sprintf(cName, "%s", RESOURCE_3_NAME);
				break;
		}
		
		//cout << "Stripping resource " << cName << endl;
		HGLOBAL hgResData = LoadResource(hExe, hResource);
		if (!hgResData) 
		{
			cout << "Error: Unable to load resource " << iResource << endl;
			return 1;
		}
		char* pBuffer = (char*)LockResource(hgResData);
		if(pBuffer == NULL) 
		{
			cout << "Error: Unable to lock resource " << iResource << endl;
			return 1;
		}

		HANDLE hFile = CreateFile(cName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile) 
		{
			cout << "Error: Unable to open output file " << cName << endl;
			return 1;
		}
		
		if(iResource != RESOURCE_1)
		{
			DWORD dwLen = 0;
			if ((!(WriteFile(hFile, pBuffer, resLen, &dwLen, NULL))) || 
			   (dwLen != resLen)) 
			{ 
				CloseHandle(hFile); 
				cout << "Error: Unable to write resource data to file " << cName << endl;
				return (1);
			}
			CloseHandle(hFile);
		}
		else	//Progress bar sort of thing for resource.pak
		{
			DWORD writtenLen = 0;
			DWORD segmentLen = resLen / 50;
			int iCurProgress = 50;
			for(;(resLen - writtenLen) > segmentLen; writtenLen += segmentLen)
			{
				DWORD dwLen = 0;
				if ((!(WriteFile(hFile, &pBuffer[writtenLen], segmentLen, &dwLen, NULL))) || 
				   (dwLen != segmentLen)) 
				{ 
					CloseHandle(hFile); 
					cout << "Error: Unable to write resource data to file " << cName << endl;
					return (1);
				}
				cout << iCurProgress++ << " percent done" << endl;
			}
			if(writtenLen < resLen)
			{
				DWORD dwLen = 0;
				if ((!(WriteFile(hFile, &pBuffer[writtenLen], (resLen - writtenLen), &dwLen, NULL))) || 
				   (dwLen != (resLen - writtenLen))) 
				{ 
					CloseHandle(hFile); 
					cout << "Error: Unable to write resource data to file " << cName << endl;
					return (1);
				}
			}
			CloseHandle(hFile);
		}
	}
	
	
	FreeLibrary(hExe);
	
	
	
	return 0;
}

int main(int argc, char** argv)
{
	cout << "Warning! This will modify your game's executable. Be sure to back it up first. Are you sure you wish to continue? (y/n) ";
	char c = 'n';
	cin >> c;
	if(c != 'y')
	{
		cout << "Abort." << endl;
		return 0;
	}

	if(pullPakFiles(argc,argv))
		return 1;
	
	HANDLE hUpdateRes;  // update resource handle
	BOOL result;
	unsigned char* cZero = (unsigned char*) malloc(4);
		
	memset(cZero, 0, 4);

	// Load the .EXE to update the resources
	hUpdateRes = BeginUpdateResource(argv[1], FALSE);
	if (hUpdateRes == NULL)
	{
		cout << "Could not open file " << argv[1] << " for writing.";
		return 1;
	}
	
	//Grab resources
	for(WORD iResource = RESOURCE_1; iResource <= RESOURCE_3; iResource++)
	{
		// Add this resource to the update list.
		result = UpdateResource(hUpdateRes, 
								MAKEINTRESOURCE(RESOURCE_TYPE), 
								MAKEINTRESOURCE(iResource), 
								MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
								cZero,
								4);

		if (result == FALSE)
		{
			cout << "Could not update resource " << iResource << ": " << GetLastError() << endl;
			return 1;
		}
	}
	
	// Finish writing changes to the executable and then close it.
	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		cout << "Could not write changes to the file " << argv[1] << endl;
		return 1;
	}
	
	//Done
	free(cZero);
	cout << "100 percent done" << endl;
	return 0;
}









