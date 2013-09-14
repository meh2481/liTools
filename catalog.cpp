#include "pakDataTypes.h"

bool catalogToXML(wstring sFilename)
{
	FILE* f = _wfopen(sFilename.c_str(), TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(sFilename) << " for reading." << endl;
		return false;
	}
	
	//Read letter header
	catalogHeader ch;
	if(fread(&ch, 1, sizeof(catalogHeader), f) != sizeof(catalogHeader))
	{
		cout << "Error: unable to read catalogHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in catalog items
	list<catalog> lCatalogs;
	for(int i = 0; i < ch.catalogs.count; i++)
	{
		catalog clog;
		if(fread(&clog, 1, sizeof(catalog), f) != sizeof(catalog))
		{
			cout << "Error: unable to read catalog from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lCatalogs.push_back(clog);
	}
	
	//Read in items
	vector<u32> vItems;
	for(int i = 0; i < ch.items.count; i++)
	{
		u32 item;
		if(fread(&item, 1, sizeof(u32), f) != sizeof(u32))
		{
			cout << "Error: unable to read catalog item from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		vItems.push_back(item);
	}
	
	//Read in string table-----------------------------------------------------------
	//Read in string table header
	StringTableHeader sth;
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), f) != sizeof(StringTableHeader))
	{
		cout << "Error: Unable to read StringTableHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}

	//Allocate memory for this many string table & pointer entries
	vector<StringTableEntry> vStringTableList;
	vector<StringPointerEntry> vStringPointerList;
	vector<char> vStringList;
	vStringTableList.reserve(sth.numStrings);
	vStringPointerList.reserve(sth.numPointers);

	//Read in string table entries
	for(int j = 0; j < sth.numStrings; j++)
	{
		StringTableEntry ste;
		if(fread((void*)&ste, 1, sizeof(StringTableEntry), f) != sizeof(StringTableEntry))
		{
			cout << "Error: Unable to read StringTableEntry " << j << " out of " << sth.numStrings << " in " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		//Store
		vStringTableList.push_back(ste);
	}

	//and string table pointers
	for(int j = 0; j < sth.numPointers; j++)
	{
		StringPointerEntry spe;
		if(fread((void*)&spe, 1, sizeof(StringPointerEntry), f) != sizeof(StringPointerEntry))
		{
			cout << "Error: Unable to read StringPointerEntry " << j << " out of " << sth.numPointers << " in " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		//Store
		vStringPointerList.push_back(spe);
	}

	//Now read in the strings until we hit the end of file
	int c;
	while((c = fgetc(f)) != EOF)
	{
		if(c == '\\')	//Change all backslashes to forward slashes.
			c = '/';
		vStringList.push_back(wchar_t(c));
	}
	//-----------------------------------------------------
	
	//Done reading
	fclose(f);
	
	//Write out to XML
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("catalogs");
	
	for(list<catalog>::iterator i = lCatalogs.begin(); i != lCatalogs.end(); i++)
	{
		XMLElement* catalog = doc->NewElement("catalog");
		catalog->SetAttribute("id", i->id);
		catalog->SetAttribute("cost", i->cost);
		catalog->SetAttribute("numCombos", i->numCombos);
		catalog->SetAttribute("bgRaysRGB", RGBToString(i->bgRaysR.value, i->bgRaysG.value, i->bgRaysB.value).c_str());
		//catalog->SetAttribute("bgRaysR", i->bgRaysR.value);
		//catalog->SetAttribute("bgRaysG", i->bgRaysG.value);
		//catalog->SetAttribute("bgRaysB", i->bgRaysB.value);
		catalog->SetAttribute("bgPaperRGB", RGBToString(i->bgPaperR.value, i->bgPaperG.value, i->bgPaperB.value).c_str());
		//catalog->SetAttribute("bgPaperR", i->bgPaperR.value);
		//catalog->SetAttribute("bgPaperG", i->bgPaperG.value);
		//catalog->SetAttribute("bgPaperB", i->bgPaperB.value);
		catalog->SetAttribute("coverTexId", ws2s(getName(i->coverTexId)).c_str());	//TODO Support new files
		catalog->SetAttribute("thumbTexId", ws2s(getName(i->thumbTexId)).c_str());
		catalog->SetAttribute("lockedTexId", ws2s(getName(i->lockedTexId)).c_str());
		
		//Write children items
		XMLElement* items = doc->NewElement("items");
		for(int j = i->firstItemIdx; j < i->firstItemIdx + i->numItems; j++)
		{
			XMLElement* item = doc->NewElement("item");
			item->SetAttribute("name", itemIDToName(vItems[j]).c_str());	//TODO Support new items
			items->InsertEndChild(item);
		}
		catalog->InsertEndChild(items);
		
		//Write name
		XMLElement* name = doc->NewElement("name");
		for(int m = vStringTableList[i->nameStrId].pointerIndex; m < vStringTableList[i->nameStrId].pointerIndex + vStringTableList[i->nameStrId].pointerCount; m++)
		{
			XMLElement* string = doc->NewElement("string");
			string->SetAttribute("lang", ws2s(toLangString(vStringPointerList[m].languageId)).c_str());
			string->SetAttribute("data", &(vStringList.data()[vStringPointerList[m].offset]));
			name->InsertEndChild(string);
		}
		catalog->InsertFirstChild(name);
		root->InsertEndChild(catalog);
	}
	
	doc->InsertFirstChild(root);
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
	
	return true;
}

bool XMLToCatalog(wstring sFilename)
{
	//Open file
	wstring sXMLFile = sFilename;
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
	list<StringTableEntry> lStringTable;
	list<StringPointerEntry> lStringPointers;
	list<char> lUTFData;
	list<catalog> lCatalogs;
	list<u32> lItems;
	for(XMLElement* catalogelem = root->FirstChildElement("catalog"); catalogelem != NULL; catalogelem = catalogelem->NextSiblingElement("catalog"))
	{
		catalog clog;
		clog.firstItemIdx = lItems.size();
		clog.numItems = 0;
		clog.bgRaysR.tuneId = clog.bgRaysG.tuneId = clog.bgRaysB.tuneId = clog.bgPaperR.tuneId = clog.bgPaperG.tuneId = clog.bgPaperB.tuneId = 0;
		if(catalogelem->QueryUnsignedAttribute("id", &clog.id) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read catalog id from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(catalogelem->QueryIntAttribute("cost", &clog.cost) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read catalog cost from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(catalogelem->QueryIntAttribute("numCombos", &clog.numCombos) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read catalog numCombos from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		const char* bgRays = catalogelem->Attribute("bgRaysRGB");
		if(bgRays == NULL)
		{
			cout << "Error: Unable to read catalog background ray color from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		RGBFromString(&clog.bgRaysR.value, &clog.bgRaysG.value, &clog.bgRaysB.value, bgRays);
		const char* bgPaper = catalogelem->Attribute("bgPaperRGB");
		if(bgRays == NULL)
		{
			cout << "Error: Unable to read catalog background paper color from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		RGBFromString(&clog.bgPaperR.value, &clog.bgPaperG.value, &clog.bgPaperB.value, bgPaper);
		const char* cover = catalogelem->Attribute("coverTexId");
		if(cover == NULL)
		{
			cout << "Error: Unable to read catalog coverTexId from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		clog.coverTexId = getResID(s2ws(cover));
		const char* thumb = catalogelem->Attribute("thumbTexId");
		if(thumb == NULL)
		{
			cout << "Error: Unable to read catalog thumbTexId from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		clog.thumbTexId = getResID(s2ws(thumb));
		const char* locked = catalogelem->Attribute("lockedTexId");
		if(locked == NULL)
		{
			cout << "Error: Unable to read catalog lockedTexId from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		clog.lockedTexId = getResID(s2ws(locked));
		
		//Read name string
		StringTableEntry nameEntry;
		nameEntry.pointerIndex = lStringPointers.size();
		nameEntry.pointerCount = 0;
		clog.nameStrId = lStringTable.size();
		XMLElement* namestr = catalogelem->FirstChildElement("name");	//Get ID string
		if(namestr == NULL)
		{
			cout << "Error reading catalog name string from file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* namestring = namestr->FirstChildElement("string"); namestring != NULL; namestring = namestring->NextSiblingElement("string"))
		{
			StringPointerEntry spe;
			const char* lang = namestring->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to read catalog id string's language from XML file " <<  ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			spe.languageId = toLangID(s2ws(lang));
			spe.offset = lUTFData.size();
			lStringPointers.push_back(spe);
			nameEntry.pointerCount++;
			
			//Read in string data
			const char* data = namestring->Attribute("data");
			if(data == NULL)
			{
				cout << "Unable to read catalog id string's data from XML file " <<  ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			for(int j = 0; j < strlen(data); j++)
				lUTFData.push_back(data[j]);
			lUTFData.push_back('\0');	//Be sure to append null character, as well
		}
		lStringTable.push_back(nameEntry);
		
		//Read children items
		XMLElement* items = catalogelem->FirstChildElement("items");	//Get ID string
		if(items == NULL)
		{
			cout << "Error reading catalog items from file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* item = items->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
		{
			u32 it;
			
			const char* itemname = item->Attribute("name");
			if(itemname == NULL)
			{
				cout << "Error reading catalog item name from file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			it = itemNameToID(itemname);
			
			lItems.push_back(it);
			clog.numItems++;
		}
		lCatalogs.push_back(clog);
	}
	
	//Done reading XML
	delete doc;
	
	//Open our output file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(sFilename) << endl;
		return false;
	}
	
	//Write out letter database header
	catalogHeader ch;
	ch.catalogs.count = lCatalogs.size();
	ch.catalogs.offset = sizeof(catalogHeader);
	ch.items.count = lItems.size();
	ch.items.offset = ch.catalogs.offset + sizeof(catalog) * ch.catalogs.count;
	ch.stringTableBytes.count = sizeof(StringTableHeader) + sizeof(StringTableEntry) * lStringTable.size() + sizeof(StringPointerEntry) * lStringPointers.size() + lUTFData.size();
	ch.stringTableBytes.offset = ch.items.offset + sizeof(u32) * ch.items.count;
	fwrite(&ch, 1, sizeof(catalogHeader), f);
	
	//Write out catalog records
	for(list<catalog>::iterator i = lCatalogs.begin(); i != lCatalogs.end(); i++)
		fwrite(&(*i), 1, sizeof(catalog), f);
		
	//Write out catalog item records
	for(list<u32>::iterator i = lItems.begin(); i != lItems.end(); i++)
		fwrite(&(*i), 1, sizeof(u32), f);
	
	//Write out string table header
	StringTableHeader sth;
	sth.numStrings = lStringTable.size();
	sth.numPointers = lStringPointers.size();
	fwrite(&sth, 1, sizeof(StringTableHeader), f);
	
	//Write out string table entries
	for(list<StringTableEntry>::iterator i = lStringTable.begin(); i != lStringTable.end(); i++)
		fwrite(&(*i), 1, sizeof(StringTableEntry), f);
		
	//Write out string table pointers
	for(list<StringPointerEntry>::iterator i = lStringPointers.begin(); i != lStringPointers.end(); i++)
		fwrite(&(*i), 1, sizeof(StringPointerEntry), f);
	
	//Write out UTF data
	for(list<char>::iterator i = lUTFData.begin(); i != lUTFData.end(); i++)
		fwrite(&(*i), 1, 1, f);
	
	//Done
	fclose(f);
	return true;
}


