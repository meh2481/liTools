#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <VFS.h>
#include <VFSTools.h>
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
	ttvfs::VFSHelper vfs;
	vfs.Prepare();
	
	cout << "Warning! This will modify your game's executable. Be sure to back it up first. Are you sure you wish to continue? (y/n) ";
	char c = 'n';
	cin >> c;
	if(c != 'y')
	{
		cout << "Abort." << endl;
		return 0;
	}
	
	HANDLE hUpdateRes;  // update resource handle
	BOOL result;

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
		string sRscName = "";
		switch(iResource)
		{
			case RESOURCE_1:
				sRscName = RESOURCE_1_NAME;
				break;
			case RESOURCE_2:
				sRscName = RESOURCE_2_NAME;
				break;
			default:
				sRscName = RESOURCE_3_NAME;
				break;
		}
		
		FILE* fp = fopen(sRscName.c_str(), "rb");
		if(fp == NULL)
		{
			cout << "Unable to open input file " << sRscName << ". Make sure all three .pak files are in the working directory. Abort." << endl;
			return 0;
		}
		
		size_t szFile = ttvfs::GetFileSize(sRscName.c_str());
		
		char* buf = (char*) malloc(szFile);
		
		if(fread(buf, 1, szFile, fp) != szFile)
		{
			cout << "Unable to read " << szFile << " bytes from file " << sRscName << "." << endl;
			free(buf);
			return 1;
		}
		
		// Add this resource to the update list.
		result = UpdateResource(hUpdateRes, 
								MAKEINTRESOURCE(RESOURCE_TYPE), 
								MAKEINTRESOURCE(iResource), 
								MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
								buf,
								szFile);

		if (result == FALSE)
		{
			cout << "Could not update resource " << iResource << ": " << GetLastError() << endl;
			free(buf);
			return 1;
		}
		
		free(buf);
	}
	
	// Finish writing changes to the executable and then close it.
	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		cout << "Could not write changes to the file " << argv[1] << endl;
		return 1;
	}
	
	//Done
	cout << "exe repacked." << endl;
	return 0;
}









