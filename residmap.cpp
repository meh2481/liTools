#include "pakDataTypes.h"
#include "residmap.h"

map<string, u32> g_repakMappings;
map<u32, string> g_pakMappings;

//Get a resource ID from a filename
u32 getResID(string sName)
{
	if(!g_repakMappings.count(sName))
	{
		//TODO: Hash filenames yadda yadda yadda
		cout << "ERROR: Invalid filename for recompression: " << sName << ". Only files in the original residmap.dat can be compressed." << endl;
		exit(1);
	}
	return g_repakMappings[sName];
}

//Get a filename from a resource ID
const char* getName(u32 resId)
{
	return g_pakMappings[resId].c_str();
	//i32 strId = g_IDMappings[resId];
	//const char* cData = g_stringList.data();
	//return &cData[g_stringPointerList[g_stringTableList[strId].pointerIndex].offset];
}

//Parse our array of values to get the mappings
void initResMap()
{
	for(u32 i = 0; i < NUM_MAPPINGS; i++)
	{
		g_repakMappings[g_residMap[i].name] = g_residMap[i].id;
		g_pakMappings[g_residMap[i].id] = g_residMap[i].name;
	}
}

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

//convert a string to lowercase. Also change forward slashes back to backslashes.
string stolower( const string s )
{
  string result = s;
  for(unsigned int i = 0; i < s.size(); i++)
  {
	char c = s[i];
	if( (c >= 'A') && (c <= 'Z') )
	{
		c += 'a' - 'A';
		result[i] = c;
	}
	if(c == '/')
		result[i] = '\\';
  }
  
  return result;
}

//Function from Allan for getting a hash from a filename
u32 LIHash( const wchar_t *pCaseInsensitiveStr )
{
	u32 hash = 0xABABABAB;
	while( *pCaseInsensitiveStr )
	{
		hash ^= *pCaseInsensitiveStr;
		hash = (hash << 7) | (hash >> (32-7));
		++pCaseInsensitiveStr;
	}

	return hash;
}

//Have to do some converting to get from std::string to wchar_t*. TODO: Native UTF-16 support or such?
u32 hash(string sFilename)
{
	//Convert to lowercase first
	return LIHash(s2ws(stolower(sFilename)).c_str());
}


//TODO: Severe problem if unknown ID and this isn't read first!!!
bool residMapToXML(const char* cFilename)
{	
	//Read in the mappings directly from residmap.dat
	FILE* fp = fopen(cFilename, "rb");
	if(fp == NULL)
	{
		cout << "Error: Unable to open file " << cFilename << endl;
		return false;
	}
	
	//Read in the headers
	ResidMapHeader rmh;
	if(fread((void*)&(rmh), 1, sizeof(ResidMapHeader), fp) != sizeof(ResidMapHeader))
	{
		cout << "ResidMapHeader malformed" << endl;
		fclose(fp);
		return false;
	}
	
	//Read in the mappings
	map<u32,i32> mIDMappings;
	fseek(fp, rmh.maps.offset, SEEK_SET);
	for(int i = 0; i < rmh.maps.count; i++)
	{
		MappingHeader mh;
		if(fread((void*)&mh, 1, sizeof(MappingHeader), fp) != sizeof(MappingHeader))
		{
			cout << "MappingHeader malformed" << endl;
			fclose(fp);
			return false;
		}
		//Store
		mIDMappings[mh.resId] = mh.strId;
	}
	
	//Now for string table header
	StringTableHeader sth;
	fseek(fp, rmh.stringTableBytes.offset, SEEK_SET);
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), fp) != sizeof(StringTableHeader))
	{
		cout << "StringTableHeader malformed" << endl;
		fclose(fp);
		return false;
	}
	
	//Allocate memory for this many string table & pointer entries
	vector<StringTableEntry> vStringTableList;
	vector<StringPointerEntry> vStringPointerList;
	vector<char> vStringList;
	vStringTableList.reserve(sth.numStrings);
	vStringPointerList.reserve(sth.numPointers);
	vStringList.reserve((sizeof(char) * sth.numStrings)*256);
	
	//Read in string table entries
	for(int i = 0; i < sth.numStrings; i++)
	{
		StringTableEntry ste;
		if(fread((void*)&ste, 1, sizeof(StringTableEntry), fp) != sizeof(StringTableEntry))
		{
			cout << "StringTableEntry " << i << " malformed out of " << sth.numStrings << endl;
			fclose(fp);
			return false;
		}
		//Store
		vStringTableList[i] = ste;
	}
	
	//and string table pointers
	for(int i = 0; i < sth.numPointers; i++)
	{
		StringPointerEntry spe;
		if(fread((void*)&spe, 1, sizeof(StringPointerEntry), fp) != sizeof(StringPointerEntry))
		{
			cout << "StringPointerEntry " << i << " malformed out of " << sth.numPointers << endl;
			fclose(fp);
			return false;
		}
		//Store
		vStringPointerList[i] = spe;
	}
	
	//Now read in the strings until we hit the end of the file
	int c;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == '\\')	//Change all backslashes to forward slashes. Tsk, tsk, Allan.
			c = '/';
		vStringList.push_back(c);
	}
	
	fclose(fp);
	
	//ofstream ofile;
	//ofile.open("residmap.txt");
	//Add these mappings to our mapping list
	for(map<u32, i32>::iterator i = mIDMappings.begin(); i != mIDMappings.end(); i++)
	{
		i32 strId = i->second;
		u32 finalNum = i->first;
		char* cData = vStringList.data();
		string s = &cData[vStringPointerList[vStringTableList[strId].pointerIndex].offset];
		//Store forward and reverse mappings for this file
		g_repakMappings[s] = finalNum;
		g_pakMappings[finalNum] = s;
		//ofile << "{" << finalNum << "u,\"" << s << "\"}," << endl;
	}
	//ofile.close();
	
	//Now save this out to XML
	string sFilename = cFilename;
	sFilename += ".xml";
	XMLDocument* doc = new XMLDocument;
	//TODO Merge with preexisting XML
	XMLElement* root = doc->NewElement("mappings");	//Create the root element
	//ofstream oHash("hash.txt");
	for(map<u32, string>::iterator i = g_pakMappings.begin(); i != g_pakMappings.end(); i++)
	{
		XMLElement* elem = doc->NewElement("mapping");
		elem->SetAttribute("id", i->first);
		elem->SetAttribute("filename", i->second.c_str());
		root->InsertEndChild(elem);
		//oHash << "id: " << i->first << ", filename: " << i->second << ", filename hashed: " << hash(i->second) << endl;
		//if(i->first != hash(i->second))
		//	oHash << "Hash failed." << endl;
		//else
		//	oHash << "Hash worked!" << endl;
	}
	//oHash.close();
	doc->InsertFirstChild(root);
	doc->SaveFile(sFilename.c_str());
	
	return true;
}

//Save residmap.dat.xml back out to residmap.dat
bool XMLToResidMap(const char* cFilename)
{
	//Open file
	string sXMLFile = cFilename;
	sXMLFile += ".xml";
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFile.c_str());
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << sXMLFile << ": Error " << iErr << endl;
		delete doc;
		return false;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << sXMLFile << endl;
		delete doc;
		return false;
	}
	
	//Read in XML data
	list<char> lUTFData;
	list<MappingHeader> lMappings;
	list<StringTableEntry> lStringTable;
	list<StringPointerEntry> lStringPointers;
	for(XMLElement* elem = root->FirstChildElement("mapping"); elem != NULL; elem = elem->NextSiblingElement("mapping"))
	{
		int id;
		if(elem->QueryIntAttribute("id", &id) != XML_NO_ERROR)
		{
			cout << "Unable to get mapping ID from XML file " << sXMLFile << endl;
			delete doc;
			return false;
		}
		const char* cName = elem->Attribute("filename");
		if(cName == NULL)
		{
			cout << "Unable to get mapping filename from XML file " << sXMLFile << endl;
			delete doc;
			return false;
		}
		//Make mapping header that maps this resource ID to the string ID
		MappingHeader mh;
		mh.resId = id;
		mh.strId = lStringTable.size();
		lMappings.push_back(mh);
		//Make StringTableEntry that maps this string ID to a string data pointer
		StringTableEntry ste;
		ste.pointerIndex = lStringPointers.size();
		ste.pointerCount = 1;
		lStringTable.push_back(ste);
		//Make the StringPointerEntry that maps this pointer to a location in the string data
		StringPointerEntry spe;
		spe.languageId = LANGID_ENGLISH;
		spe.offset = lUTFData.size();
		lStringPointers.push_back(spe);
		//Add this string to our string list
		unsigned int iStrLen = strlen(cName)+1;	//+1 so we can keep the terminating \0 character
		for(unsigned int i = 0; i < iStrLen; i++)
			lUTFData.push_back(cName[i]);			//Copy data over
	}
	delete doc;	//Done reading XML
	
	//Open our output file
	FILE* f = fopen(cFilename, "wb");
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << cFilename << endl;
		return false;
	}
	
	//Write out our ResidMapHeader
	ResidMapHeader rmh;
	size_t curOffset = sizeof(ResidMapHeader);
	rmh.maps.count = lMappings.size();
	rmh.maps.offset = curOffset;
	curOffset += sizeof(MappingHeader) * lMappings.size();
	//The count for this is the number of bytes for all of it
	rmh.stringTableBytes.count = sizeof(StringTableHeader) + sizeof(StringTableEntry) * lStringTable.size() + lUTFData.size();
	rmh.stringTableBytes.offset = curOffset;
	
	fwrite(&rmh, 1, sizeof(ResidMapHeader), f);
	
	//Write out our MappingHeaders
	for(list<MappingHeader>::iterator i = lMappings.begin(); i != lMappings.end(); i++)
		fwrite(&(*i), 1, sizeof(MappingHeader), f);
		
	//Write out our StringTableHeader
	StringTableHeader sth;
	sth.numStrings = lStringTable.size();
	sth.numPointers = lStringPointers.size();
	fwrite(&sth, 1, sizeof(StringTableHeader), f);
	
	//Write out our StringTableEntries
	for(list<StringTableEntry>::iterator i = lStringTable.begin(); i != lStringTable.end(); i++)
		fwrite(&(*i), 1, sizeof(StringTableEntry), f);
	
	//Write out our StringPointerEntries
	for(list<StringPointerEntry>::iterator i = lStringPointers.begin(); i != lStringPointers.end(); i++)
		fwrite(&(*i), 1, sizeof(StringPointerEntry), f);
	
	//Write out our string data
	for(list<char>::iterator i = lUTFData.begin(); i != lUTFData.end(); i++)
		fwrite(&(*i), 1, 1, f);
		
	fclose(f);	//Done
	
	return true;
}















