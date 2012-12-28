#include "pakDataTypes.h"

ttvfs::VFSHelper vfs;

extern list<ThreadConvertHelper> g_lThreadedResources;	//Declared in threadCompress.cpp, used for multi-threaded compression
extern bool g_bProgressOverwrite;
extern unsigned int g_iNumThreads;

map<string, pakHelper> g_pakHelping;

//Remove files in the temp/ folder, recursively
void removeTempFiles()
{
	ttvfs::StringList slFiles;
    ttvfs::GetFileList("temp", slFiles);

    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    {
		string s = "temp/" + (*il);
		unlink(s.c_str());	//Remove this file
    }
	rmdir("temp/");	//Remove the folder itself
}

void parseCmdLine(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		if(s == "--overwrite-progress")
			g_bProgressOverwrite = true;
		else if(s.find("--threads=") != string::npos)
		{
			size_t pos = s.find('=')+1;
			if(s.length() <= pos)
			{
				cout << "missing thread count" << endl;
				continue;
			}
			int iNumThreads = atoi(&s.c_str()[pos]);
			if(iNumThreads < 0 ||
			   iNumThreads > MAX_NUM_THREADS)
			{
				cout << "Invalid number of threads: " << iNumThreads << endl;
				continue;
			}
			g_iNumThreads = iNumThreads;
		}
		else if(argv[i][0] == '-')
			cout << "Unknown commandline switch " << argv[i] << ". Ignoring..." << endl;
	}
}

//Main program entry point
int main(int argc, char** argv)
{
	g_bProgressOverwrite = false;
	g_iNumThreads = 0;
	DWORD iTicks = GetTickCount();

	vfs.Prepare();
		
	//read in the resource names to pack
	initResMap();
	initSoundManifest();
	parseCmdLine(argc, argv);
	
	if(argc < 2)
	{
		cout << "Usage: liCompress [pakfile1] [pakfile2] ... [pakfileN]" << endl;
		return 0;
	}
	
	for(int iArg = 1; iArg < argc; iArg++)
	{
		if(argv[iArg][0] == '-')	//Skip over commandline switches
			continue;
		cout << endl << "Packing resource blob file " << argv[iArg] << endl;
		
		//Determine what files to pack into this .pak file
		list<string> lstrFilesToPak;
		string sInfilename = argv[iArg];
		sInfilename += ".filelist.txt";
		ifstream infile(sInfilename.c_str());
		if(infile.fail())
		{
			cout << "Cannot open " << sInfilename << " to pack " << argv[iArg] << " Skipping..." << endl;
			continue;
		}
		while(!infile.fail() && !infile.eof())	//Pull in all the lines out of this file
		{
			string s;
			getline(infile, s);
			if(!s.length() || s == "")
				continue;	//Ignore blank lines
			//Convert \\ characters to /
			size_t found = s.find_first_of('\\');
			while (found != string::npos)
			{
				s[found] = '/';
				found = s.find_first_of('\\', found+1);
			}
			lstrFilesToPak.push_back(s);
		}
		
		//Pull in each file
		removeTempFiles();
		if(!ttvfs::IsDirectory("temp"))
			ttvfs::CreateDirRec("temp");
		int iCurFilePaking = 0;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{
			iCurFilePaking++;
			u32 repakName = getResID(*i);
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", repakName);
			
			ThreadConvertHelper tch;
			tch.sIn = *i;
			tch.sFilename = cIDFilename;
			//We don't care about compression for this tch
			g_lThreadedResources.push_back(tch);	//Add this to our queue to compress
		}
		
		threadedCompress();	//Compress everything
		
		//Ok, now we have all the compressed files in temp/ . Stick them in the .pak file and call it a day
		ttvfs::StringList slFiles;
		ttvfs::GetFileList("temp", slFiles);
		if(slFiles.size() != lstrFilesToPak.size())	//These should be the same
		{
			cout << "Error: size of file list: " << slFiles.size() << " differs from size of files to pak: " << lstrFilesToPak.size() << endl;
			continue;
		}
		
		//Open our output pakfile for writing
		FILE* f = fopen(argv[iArg], "wb");
		if(f == NULL)
		{
			cout << "Unable to open file " << argv[iArg] << " for writing. Skipping..." << endl;
			continue;
		}
		
		//Add the header
		blobHeader bh;
		bh.pakVersion = 0x01;
		bh.numItems = slFiles.size();
		
		fwrite(&bh, 1, sizeof(blobHeader), f);
		
		//Get the starting file pos for where we (should) be writing objects to
		size_t offsetPos = sizeof(blobHeader) + (lstrFilesToPak.size() * sizeof(resourceHeader));	
		
		//Add the table of contents
		cout << "\rAdding table of contents...                " << endl;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{
			resourceHeader rH;
			rH.id = getResID(*i);
			g_pakHelping[*i].id = rH.id;	//Hang onto this for later
			rH.flags = 0x01;
			
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", rH.id);
			
			rH.offset = offsetPos;	//Offset
			if(g_pakHelping[*i].bCompressed)
				rH.size = ttvfs::GetFileSize(cIDFilename) + sizeof(compressedHeader);	//Size with compressed header
			else
			{
				rH.size = ttvfs::GetFileSize(cIDFilename);	//Size without compressed header
				rH.flags = 0x00;	//Set the flags to uncompressed
			}
			
			fwrite(&rH, 1, sizeof(resourceHeader), f);	//Write this to the file
			
			offsetPos += rH.size;	//Add this size to our running tally of where we are
		}
		
		//Add actual resource data
		cout << "Adding compressed files..." << endl;
		for(list<string>::iterator i = lstrFilesToPak.begin(); i != lstrFilesToPak.end(); i++)
		{			
			pakHelper pH = g_pakHelping[*i];
			
			//Write the compressed header (Only if compressed)
			if(pH.bCompressed)
				fwrite(&(pH.cH), 1, sizeof(compressedHeader), f);
			
			//Write this file
			char cIDFilename[256];
			sprintf(cIDFilename, "temp/%u", pH.id);
			
			size_t fileSize = ttvfs::GetFileSize(cIDFilename);
			char* cTemp = (char*)malloc(fileSize);
			FILE* fTempIn = fopen(cIDFilename, "rb");
			if(fTempIn == NULL)
			{
				cout << "Error opening " << cIDFilename << endl;
				continue;
			}
			
			if(fread(cTemp, 1, fileSize, fTempIn) != fileSize)
			{
				cout << "Error reading from " << cIDFilename << endl;
				continue;
			}
			
			fwrite(cTemp, 1, fileSize, f);	//Write this to the file
			
			free(cTemp);
			fclose(fTempIn);
		}
		fclose(f);	//Done packing this .pak file
		removeTempFiles();
	}
	removeTempFiles();
	cout << "Done." << endl;
	
	iTicks = GetTickCount() - iTicks;
	int iSeconds = iTicks / 1000;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	
	return 0;
}
















