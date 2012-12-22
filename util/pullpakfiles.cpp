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

int main(int argc, char** argv)
{
	HMODULE hExe;        // handle to .EXE file
	
	if(argc != 2)
	{
		cout << "Usage: pullpakfiles.exe [little inferno executable filename]" << endl;
	}

	// Load the .EXE whose resources you want to list.
	hExe = LoadLibrary(TEXT(argv[1]));
	if (hExe == NULL)
	{
		cout << "Unable to load " << argv[1] << endl;
		return 1;
	}
	
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

		HANDLE hFile = CreateFile(cName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile) 
		{
			cout << "Error: Unable to open output file " << cName << endl;
			return 1;
		}
		
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
	
	
	
	
	
	
	return 0;
}