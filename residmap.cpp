#include "pakDataTypes.h"
#include "residmap.h"

//#define RESIDMAP_FILENAME	"vdata/residmap.dat.xml"

map<wstring, u32> g_repakMappings;
map<u32, wstring> g_pakMappings;

//Get a resource ID from a filename
u32 getResID(wstring sName)
{
	if(!g_repakMappings.count(sName))
	{
		//TODO: Hash filenames yadda yadda yadda
		//cout << "ERROR: Invalid filename for recompression: " << ws2s(sName) << ". Only files in the original residmap.dat can be compressed." << endl;
		return 0;
	}
	return g_repakMappings[sName];
}

//Get a filename from a resource ID
const wchar_t* getName(u32 resId)
{
	if(!g_pakMappings.count(resId))
		cout << "No residmap entry for id " << resId << endl;
	return g_pakMappings[resId].c_str();
}

//Parse our array of values to get the mappings
void initResMap()
{
	for(u32 i = 0; i < NUM_MAPPINGS; i++)
	{
		g_repakMappings[s2ws(g_residMap[i].name)] = g_residMap[i].id;
		g_pakMappings[g_residMap[i].id] = s2ws(g_residMap[i].name);
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
wstring stolower( const wstring s )
{
  wstring result = s;
  for(unsigned int i = 0; i < s.size(); i++)
  {
	wchar_t c = s[i];
	if( (c >= L'A') && (c <= 'Z') )
	{
		c += L'a' - L'A';
		result[i] = c;
	}
	if(c == L'/')
		result[i] = L'\\';
  }
  
  return result;
}

//Converts forward slashes to backslashes
wstring toBackslashes(const wstring s)
{
  wstring result = s;
  for(unsigned int i = 0; i < s.size(); i++)
  {
	wchar_t c = s[i];
	if(c == L'/')
		result[i] = L'\\';
  }
  
  return result;
}

//Function from Allan for getting a hash from a filename NOTE: Don't use directly! Use hash() instead
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

//Have to do some converting to get from std::wstring to wchar_t*
u32 hash(wstring sFilename)
{
	//Convert to lowercase first
	return LIHash(stolower(sFilename).c_str());
}

//Read in mappings from a residmap.dat file
bool parseResidMap(const wchar_t* cFilename)
{
	//Read in the mappings directly from residmap.dat
	FILE* fp = _wfopen(cFilename, TEXT("rb"));
	if(fp == NULL)
	{
		cout << "Error: Unable to open file " << ws2s(cFilename) << endl;
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
	vector<wchar_t> vStringList;
	vStringTableList.reserve(sth.numStrings);
	vStringPointerList.reserve(sth.numPointers);
	vStringList.reserve((sizeof(wchar_t) * sth.numStrings)*256);
	
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
	
	//Add these mappings to our mapping list
	for(map<u32, i32>::iterator i = mIDMappings.begin(); i != mIDMappings.end(); i++)
	{
		i32 strId = i->second;
		u32 finalNum = i->first;
		wchar_t* cData = vStringList.data();
		wstring s = &cData[vStringPointerList[vStringTableList[strId].pointerIndex].offset];
		//Store forward and reverse mappings for this file
		g_repakMappings[s] = finalNum;
		g_pakMappings[finalNum] = s;
	}
	
	return true;
}

//TODO: Severe problem if unknown ID and this isn't read first. Write small residmap.dat files to beginnings of pakfiles
bool residMapToXML(const wchar_t* cFilename)
{	
	//Pull our mappings out of this file
	if(!parseResidMap(cFilename))
		return false;
	
	//Now save this out to XML
	wstring sFilename = cFilename;
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(ws2s(sFilename).c_str());
	XMLElement* root;
	map<u32,wstring> mCurXMLMappings;
	if(iErr != XML_NO_ERROR)
	{
		// residmap.xml isn't here or is malformed; overwrite
		delete doc;
		doc = new XMLDocument;
		root = doc->NewElement("mappings");	//Create the root element
		doc->InsertFirstChild(root);
	}
	else
	{
		root = doc->RootElement();	//Get the root element
		//Parse all elements in the XML and populate our map
		for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
		{
			u32 id = 0;
			elem->QueryUnsignedAttribute("id", &id);
			if(id != 0)
				mCurXMLMappings[id] = s2ws(elem->Attribute("filename"));
		}
	}
	//Merge with preexisting XML
	for(map<u32, wstring>::iterator i = g_pakMappings.begin(); i != g_pakMappings.end(); i++)
	{
		if(mCurXMLMappings.count(i->first))
		{
			if(mCurXMLMappings[i->first] != i->second)
			{
				cout << "Error: Conflict in IDs in residMapToXML(). Abort." << endl;
				delete doc;
				return false;
			}
			continue;
		}
		XMLElement* elem = doc->NewElement("mapping");
		elem->SetAttribute("id", i->first);
		elem->SetAttribute("filename", ws2s(i->second).c_str());
		root->InsertEndChild(elem);
	}
	
	//Done
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
	return true;
}

//Save residmap.dat.xml back out to residmap.dat
bool XMLToResidMap(const wchar_t* cFilename)
{
	//Open file
	wstring sXMLFile = cFilename;
	sXMLFile += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(ws2s(sXMLFile).c_str());
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << ws2s(sXMLFile) << ": Error " << iErr << endl;
		delete doc;
		return false;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << ws2s(sXMLFile) << endl;
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
			cout << "Unable to get mapping ID from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		const char* cName = elem->Attribute("filename");
		if(cName == NULL)
		{
			cout << "Unable to get mapping filename from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		//Make mapping header that maps this resource ID to the wstring ID
		MappingHeader mh;
		mh.resId = id;
		mh.strId = lStringTable.size();
		lMappings.push_back(mh);
		//Make StringTableEntry that maps this wstring ID to a wstring data pointer
		StringTableEntry ste;
		ste.pointerIndex = lStringPointers.size();
		ste.pointerCount = 1;
		lStringTable.push_back(ste);
		//Make the StringPointerEntry that maps this pointer to a location in the wstring data
		StringPointerEntry spe;
		spe.languageId = LANGID_ENGLISH;
		spe.offset = lUTFData.size();
		lStringPointers.push_back(spe);
		//Add this string to our string list
		const char* cFile = ws2s(toBackslashes(s2ws(cName))).c_str();
		unsigned int iStrLen = strlen(cFile)+1;	//+1 so we can keep the terminating \0 character
		for(unsigned int i = 0; i < iStrLen; i++)
			lUTFData.push_back(cFile[i]);			//Copy data over
	}
	delete doc;	//Done reading XML
	
	//Open our output file
	FILE* f = _wfopen(cFilename, TEXT("wb"));
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
	
	//Write out our wstring data
	for(list<char>::iterator i = lUTFData.begin(); i != lUTFData.end(); i++)
		fwrite(&(*i), 1, 1, f);
		
	fclose(f);	//Done
	
	return true;
}















