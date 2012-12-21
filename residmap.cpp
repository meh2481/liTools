#include "pakDataTypes.h"
#include <map>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cstring>
using namespace std;

map<u32,i32> g_IDMappings;
vector<StringTableEntry> g_stringTableList;
vector<StringPointerEntry> g_stringPointerList;
vector<char> g_stringList;
map<string, u32> g_repakMappings;

//Get a resource ID from a filename
u32 getResID(string sName)
{
	return g_repakMappings[sName];
}

//Get a filename from a resource ID
const char* getName(u32 resId)
{
	i32 strId = g_IDMappings[resId];
	const char* cData = g_stringList.data();
	return &cData[g_stringPointerList[g_stringTableList[strId].pointerIndex].offset];
}

//Read in in our decompressed "residmap.dat" file, which includes the filenames for us to stick them in the right place
void readResidMap()
{
	FILE* fp = fopen("residmap.dat", "rb");	//This is "debug.pak" already in extracted form in the file "residmap.dat", for my ease of use.
	if(fp == NULL)
	{
		cout << "Unable to open residmap.dat. Please place this file in the same directory as this program. Abort." << endl;
		exit(0);
	}
	
	DebugPakHeader dph;
	
	//Read in the headers
	if(fread((void*)&(dph), 1, sizeof(DebugPakHeader), fp) != sizeof(DebugPakHeader))
	{
		cout << "DebugPakHeader malformed" << endl;
		fclose(fp);
		exit(0);
	}
	
	//Read in the mappings
	for(int i = 0; i < dph.maps.count; i++)
	{
		MappingHeader mh;
		if(fread((void*)&mh, 1, sizeof(MappingHeader), fp) != sizeof(MappingHeader))
		{
			cout << "MappingHeader malformed" << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_IDMappings[mh.resId] = mh.strId;
	}
	
	//Now for string table header
	StringTableHeader sth;
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), fp) != sizeof(StringTableHeader))
	{
		cout << "StringTableHeader malformed" << endl;
		fclose(fp);
		exit(0);
	}
	//Allocate memory for this many string table & pointer entries
	g_stringTableList.reserve(sth.numStrings);
	g_stringPointerList.reserve(sth.numPointers);
	g_stringList.reserve((sizeof(char) * sth.numStrings)*256);
	
	//Read in string table entries
	for(int i = 0; i < sth.numStrings; i++)
	{
		StringTableEntry ste;
		if(fread((void*)&ste, 1, sizeof(StringTableEntry), fp) != sizeof(StringTableEntry))
		{
			cout << "StringTableEntry " << i << " malformed out of " << sth.numStrings << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_stringTableList[i] = ste;
	}
	
	//and string table pointers
	for(int i = 0; i < sth.numPointers; i++)
	{
		StringPointerEntry spe;
		if(fread((void*)&spe, 1, sizeof(StringPointerEntry), fp) != sizeof(StringPointerEntry))
		{
			cout << "StringPointerEntry malformed" << endl;
			fclose(fp);
			exit(0);
		}
		//Store
		g_stringPointerList[i] = spe;
	}
	
	//Now read in the strings until we hit the end of the file
	int c;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == '\\')	//Change all backslashes to forward slashes. Tsk, tsk, Allan.
			c = '/';
		g_stringList.push_back(c);
	}
	
	fclose(fp);
	
	//Reverse mappings so we can access string ID's easily from the string filename
	for(map<u32, i32>::iterator i = g_IDMappings.begin(); i != g_IDMappings.end(); i++)
	{
		i32 strId = i->second;
		u32 finalNum = i->first;
		char* cData = g_stringList.data();
		string s = &cData[g_stringPointerList[g_stringTableList[strId].pointerIndex].offset];
		g_repakMappings[s] = finalNum;
	}
}