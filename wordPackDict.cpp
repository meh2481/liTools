#include <tinyxml2.h>
#include "pakDataTypes.h"
#include <cstring>
#include <string>
#include <list>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
using namespace tinyxml2;

bool wordPackToXML(const char* cFilename)
{
	//Load the data from this file
	FILE* f = fopen(cFilename, "rb");
	if(f == NULL)
	{
		cout << "Unable to open " << cFilename << " for reading." << endl;
		return false;
	}
	
	//Read header
	wordPackDictHeader wpHeader;
	if(fread(&wpHeader, 1, sizeof(wordPackDictHeader), f) != sizeof(wordPackDictHeader))
	{
		cout << "Unable to read wordPackDictHeader." << endl;
		fclose(f);
		return false;
	}
	
	list<wordHeader> whHeaders;
	//Read in wordHeaders
	for(i32 i = 0; i < wpHeader.words.count; i++)
	{
		wordHeader wh;
		if(fread(&wh, 1, sizeof(wordHeader), f) != sizeof(wordHeader))
		{
			cout << "Unable to read wordHeader " << i << endl;
			fclose(f);
			return false;
		}
		whHeaders.push_back(wh);
	}
	
	//Now read in string table
	//Now for string table header
	StringTableHeader sth;
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), f) != sizeof(StringTableHeader))
	{
		cout << "StringTableHeader malformed" << endl;
		fclose(f);
		return false;
	}
	
	vector<StringTableEntry> stringTableList;
	vector<StringPointerEntry> stringPointerList;
	vector<char> stringList;
	
	//Allocate memory for this many string table & pointer entries
	stringTableList.reserve(sth.numStrings);
	stringPointerList.reserve(sth.numPointers);
	stringList.reserve((sizeof(char) * sth.numStrings)*256);
	
	//Read in string table entries
	for(int i = 0; i < sth.numStrings; i++)
	{
		StringTableEntry ste;
		if(fread((void*)&ste, 1, sizeof(StringTableEntry), f) != sizeof(StringTableEntry))
		{
			cout << "StringTableEntry " << i << " malformed out of " << sth.numStrings << endl;
			fclose(f);
			return false;
		}
		//Store
		stringTableList[i] = ste;
	}
	
	//and string table pointers
	for(int i = 0; i < sth.numPointers; i++)
	{
		StringPointerEntry spe;
		if(fread((void*)&spe, 1, sizeof(StringPointerEntry), f) != sizeof(StringPointerEntry))
		{
			cout << "StringPointerEntry malformed" << endl;
			fclose(f);
			return false;
		}
		//Store
		stringPointerList[i] = spe;
	}
	
	//Now read in the strings until we hit the end of the file
	int c;
	while((c = fgetc(f)) != EOF)
		stringList.push_back(c);
		
	//Save to XML
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("words");	//Create the root element
	root->SetAttribute("num", whHeaders.size());
	
	for(list<wordHeader>::iterator i = whHeaders.begin(); i != whHeaders.end(); i++)
	{
		char* cData = stringList.data();
		string sWord = &cData[stringPointerList[stringTableList[i->wordStrId].pointerIndex].offset];
		
		XMLElement* elem = doc->NewElement("word");
		elem->SetAttribute("str", sWord.c_str());
		elem->SetAttribute("probability", i->probability);
		root->InsertEndChild(elem);
	}
	
	doc->InsertFirstChild(root);
	string sFilename = cFilename;
	sFilename += ".xml";
	doc->SaveFile(sFilename.c_str());
	
	delete doc;
	fclose(f);
	
	return true;
}

bool XMLToWordPack(const char* cFilename)
{
	
}









