
#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;

HANDLE g_hFile;   // global handle to resource info file
// Declare callback functions.
BOOL EnumTypesFunc(
       HMODULE hModule,
       LPTSTR lpType,
       LONG lParam);
   
BOOL EnumNamesFunc(
       HMODULE hModule,
       LPCTSTR lpType,
       LPTSTR lpName,
       LONG lParam);
   
/*BOOL EnumLangsFunc(
       HMODULE hModule,
       LPCTSTR lpType,
       LPCTSTR lpName,
       WORD wLang,
       LONG lParam);*/


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#ifdef main
    #undef main     //To avoid Windows errors when it expects WinMain() instead
#endif
int main(int argc, char** argv)
{

	if(argc != 2)
	{
		cout << "Usage: WinResource.exe [executable]" << endl;
		return 0;
	}

	HMODULE hExe;        // handle to .EXE file
	TCHAR szBuffer[80];  // print buffer for info file
	DWORD cbWritten;     // number of bytes written to resource info file
	//size_t cbString;     // length of string in szBuffer
	HRESULT hResult;

	// Load the .EXE whose resources you want to list.
	hExe = LoadLibrary(TEXT(argv[1]));
	if (hExe == NULL)
	{
		// Add code to fail as securely as possible.
		cout << "Unable to load " << argv[1] << endl;
		return 0;
	}

	// Create a file to contain the resource info.
	g_hFile = CreateFile(TEXT("resinfo.txt"),   // name of file
		GENERIC_READ | GENERIC_WRITE,      // access mode
		0,                                 // share mode
		(LPSECURITY_ATTRIBUTES) NULL,      // default security
		CREATE_ALWAYS,                     // create flags
		FILE_ATTRIBUTE_NORMAL,             // file attributes
		(HANDLE) NULL);                    // no template
	if (g_hFile == INVALID_HANDLE_VALUE)
	{
		cout << "Could not open file" << endl;
		return 0;
	}

	// Find all of the loaded file's resources.
	sprintf(szBuffer, 
		TEXT("The file contains the following resources:\r\n\r\n"));

	WriteFile(g_hFile,       // file to hold resource info
		szBuffer,            // what to write to the file
		strlen(szBuffer),    // number of bytes in szBuffer
		&cbWritten,          // number of bytes written
		NULL);               // no overlapped I/O

	EnumResourceTypes(hExe,              // module handle
		(ENUMRESTYPEPROC)EnumTypesFunc,  // callback function
		0);                              // extra parameter

	// Unload the executable file whose resources were
	// enumerated and close the file created to contain
	// the resource information.
	FreeLibrary(hExe);
	CloseHandle(g_hFile);


}


//    FUNCTION: EnumTypesFunc(HANDLE, LPSTR, LONG)
//
//    PURPOSE:  Resource type callback
BOOL EnumTypesFunc(
        HMODULE hModule,  // module handle
        LPTSTR lpType,    // address of resource type
        LONG lParam)      // extra parameter, could be
                          // used for error checking
{
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    //size_t cbString;
    HRESULT hResult;

    // Write the resource type to a resource information file.
    // The type may be a string or an unsigned decimal
    // integer, so test before printing.
    if (!IS_INTRESOURCE(lpType))
    {
        sprintf(szBuffer, TEXT("Type: %s\r\n"), lpType);
		//cout << "!isresource" << endl;
    }
    else
    {
        sprintf(szBuffer, TEXT("Type: %u\r\n"), (ULONG)lpType);
		//cout << "isresource" << endl;
    }

    WriteFile(g_hFile, szBuffer, strlen(szBuffer), &cbWritten, NULL);
    // Find the names of all resources of type lpType.
    EnumResourceNames(hModule,
        lpType,
        (ENUMRESNAMEPROC)EnumNamesFunc,
        0);

    return TRUE;
}

//    FUNCTION: EnumNamesFunc(HANDLE, LPSTR, LPSTR, LONG)
//
//    PURPOSE:  Resource name callback
BOOL EnumNamesFunc(
        HMODULE hModule,  // module handle
        LPCTSTR lpType,   // address of resource type
        LPTSTR lpName,    // address of resource name
        LONG lParam)      // extra parameter, could be
                          // used for error checking
{
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    //size_t cbString;
    HRESULT hResult;
	HRSRC hResInfo;

    // Write the resource name to a resource information file.
    // The name may be a string or an unsigned decimal
    // integer, so test before printing.
    if (!IS_INTRESOURCE(lpName))
    {
        sprintf(szBuffer, TEXT("\tName: %s\r\n"), lpName);
		//cout << "!isresource" << endl;
    }
    else
    {
        sprintf(szBuffer, TEXT("\tName: %u\r\n"), (ULONG)lpName);
		//cout << "isresource" << endl;
    }
   
    WriteFile(g_hFile, szBuffer, strlen(szBuffer), &cbWritten, NULL);
	
	hResInfo = FindResource(hModule, lpName, lpType);
	if(!hResInfo)
	{
		cout << "No resource by that name" << endl;
		return TRUE;
	}
	DWORD resLen = SizeofResource(hModule, hResInfo);
	if (!resLen)
	{
		cout << "Error: size of resource = 0" << endl;
		return TRUE;
	}
	
    // Write the resource handle and size to buffer.
    sprintf(szBuffer,
			TEXT("\t\thResInfo == %lx,  Size == %lu\r\n\r\n"),
			hResInfo,
			resLen);
	WriteFile(g_hFile, szBuffer, strlen(szBuffer), &cbWritten, NULL);

    return TRUE;
}

//    FUNCTION: EnumLangsFunc(HANDLE, LPSTR, LPSTR, WORD, LONG)
//
//    PURPOSE:  Resource language callback
/*BOOL EnumLangsFunc(
        HMODULE hModule, // module handle
        LPCTSTR lpType,  // address of resource type
        LPCTSTR lpName,  // address of resource name
        WORD wLang,      // resource language
        LONG lParam)     // extra parameter, could be
                         // used for error checking
{
    HRSRC hResInfo;
    TCHAR szBuffer[80];  // print buffer for info file
    DWORD cbWritten;     // number of bytes written to resource info file
    //size_t cbString;
    HRESULT hResult;

    hResInfo = FindResourceEx(hModule, lpType, lpName, wLang);
    // Write the resource language to the resource information file.
    sprintf(szBuffer, TEXT("\t\tLanguage: %s\r\n"), (USHORT) wLang);

    WriteFile(g_hFile, szBuffer, strlen(szBuffer), &cbWritten, NULL); 
    // Write the resource handle and size to buffer.
    sprintf(szBuffer,
        TEXT("\t\thResInfo == %lx,  Size == %lu\r\n\r\n"),
        hResInfo,
        SizeofResource(hModule, hResInfo));

    WriteFile(g_hFile, szBuffer, strlen(szBuffer), &cbWritten, NULL);
    return TRUE;
}*/


