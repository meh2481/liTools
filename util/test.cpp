#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <VFS.h>
#include <VFSTools.h>
using namespace std;

int main(int argc, char** argv)
{
	ttvfs::VFSHelper vfs;
	ttvfs::StringList slFiles;
    ttvfs::GetFileListRecursive("data", slFiles, true);
    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
		cout << (*il) << endl;
	slFiles.clear();
	ttvfs::GetFileListRecursive("vdata", slFiles, true);
    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
		cout << (*il) << endl;
	
	return 0;
}