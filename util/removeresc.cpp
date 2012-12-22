#include <windows.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

#define RESOURCE_TYPE	20480

#define RESOURCE_1	1
#define RESOURCE_2	2
#define RESOURCE_3	3

int main(int argc, char** argv)
{
	HANDLE hUpdateRes;  // update resource handle
	BOOL result;
	unsigned char* cZero = (unsigned char*) malloc(4);
		
	memset(cZero, 0, 4);
	
	if(argc != 2)
	{
		cout << "Usage: removeresc.exe [little inferno executable filename]" << endl;
		return 1;
	}
	
	cout << "Warning! This will modify your game's executable. Be sure to back it up first. Are you sure you wish to continue? (y/n) ";
	char c = 'n';
	cin >> c;
	if(c != 'y')
	{
		cout << "Abort." << endl;
		return 0;
	}

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
	cout << "Exe stripped." << endl;
	return 0;
}











