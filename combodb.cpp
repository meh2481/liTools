#include "pakDataTypes.h"
#include "itemnames.h"
#include <sstream>

bool comboDBToXML(const wchar_t* cFilename)
{	
	map<u32, string> m_itemNames;
	
	//Set up map to get item names from IDs easily
	//TODO: Support new items here
	for(int i = 0; i < NUM_ITEMNAMES; i++)
		m_itemNames[g_itemNames[i].id] = g_itemNames[i].name;

	//Open the file
	FILE* f = _wfopen(cFilename, TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(cFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in the header
	comboHeader ch;
	if(fread(&ch, 1, sizeof(comboHeader), f) != sizeof(comboHeader))
	{
		cout << "Error: unable to read comboHeader from file " << ws2s(cFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in combos
	vector<comboRecord> vCombos;
	for(int i = 0; i < ch.combos.count; i++)
	{
		comboRecord cr;
		if(fread(&cr, 1, sizeof(comboRecord), f) != sizeof(comboRecord))
		{
			cout << "Error: unable to read comboRecord from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vCombos.push_back(cr);
	}
	cout << "Done reading combos" << endl;
	
	//Read in combo items
	vector<comboItemRecord> vComboItems;
	for(int i = 0; i < ch.items.count; i++)
	{
		comboItemRecord cir;
		if(fread(&cir, 1, sizeof(comboItemRecord), f) != sizeof(comboItemRecord))
		{
			cout << "Error: unable to read comboItemRecord from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vComboItems.push_back(cir);
	}
	
	cout << "Done reading items" << endl;
	
	//Read in string table-----------------------------------------------------------
	//Read in string table header
	StringTableHeader sth;
	if(fread((void*)&sth, 1, sizeof(StringTableHeader), f) != sizeof(StringTableHeader))
	{
		cout << "Error: Unable to read StringTableHeader from file " << ws2s(cFilename) << endl;
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
			cout << "Error: Unable to read StringTableEntry " << j << " out of " << sth.numStrings << " in " << ws2s(cFilename) << endl;
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
			cout << "Error: Unable to read StringPointerEntry " << j << " out of " << sth.numPointers << " in " << ws2s(cFilename) << endl;
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
	
	//Done with file
	fclose(f);
	
	//Write out to XML
	wstring sFilename = cFilename;
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("combos");
	
	for(int i = 0; i != vCombos.size(); i++)
	{
		XMLElement* elem = doc->NewElement("combo");
		elem->SetAttribute("id", vCombos[i].id);	//TODO: What for???
		elem->SetAttribute("coinvalue", vCombos[i].value);
		elem->SetAttribute("stampvalue", vCombos[i].stampValue);
		
		//Write combo name
		XMLElement* elem2 = doc->NewElement("name");
		elem2->SetAttribute("strid", vCombos[i].title.id);	//TODO: What for???
		//elem2->SetAttribute("stringkey", vCombos[i].title.key);
		for(int j = vStringTableList[vCombos[i].title.key].pointerIndex; j < vStringTableList[vCombos[i].title.key].pointerIndex + vStringTableList[vCombos[i].title.key].pointerCount; j++)
		{
			XMLElement* elem3 = doc->NewElement("string");
			elem3->SetAttribute("lang", ws2s(toLangString(vStringPointerList[j].languageId)).c_str());
			elem3->SetAttribute("data", &(vStringList.data()[vStringPointerList[j].offset]));
			elem2->InsertEndChild(elem3);
		}
		elem->InsertEndChild(elem2);
		
		//Write items in this combo
		if(vCombos[i].numItems > 0)
		{
			XMLElement* elem3 = doc->NewElement("items");
			for(int j = vCombos[i].firstItemIdx; j < vCombos[i].firstItemIdx + vCombos[i].numItems; j++)
			{
				XMLElement* elem4 = doc->NewElement("item");
				//elem4->SetAttribute("id", vComboItems[j].itemId);
				elem4->SetAttribute("name", m_itemNames[vComboItems[j].itemId].c_str());
				ostringstream oss;
				oss << vComboItems[j].picTexX << ", " << vComboItems[j].picTexY;
				elem4->SetAttribute("texpos", oss.str().c_str());
				//elem4->SetAttribute("texx", vComboItems[j].picTexX);
				//elem4->SetAttribute("texy", vComboItems[j].picTexY);
				oss.str("");
				oss << vComboItems[j].picTexW << ", " << vComboItems[j].picTexH;
				elem4->SetAttribute("texsize", oss.str().c_str());
				//elem4->SetAttribute("texw", vComboItems[j].picTexW);
				//elem4->SetAttribute("texh", vComboItems[j].picTexH);
				elem3->InsertEndChild(elem4);
			}
			elem->InsertEndChild(elem3);
		}
		root->InsertEndChild(elem);
	}
	doc->InsertFirstChild(root);
	
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
	return true;
}

bool XMLToComboDB(const wchar_t* cFilename)
{
	map<string, u32> m_itemNames;
	
	//Set up map to get item IDs from names easily
	//TODO: Support new items here
	for(int i = 0; i < NUM_ITEMNAMES; i++)
		m_itemNames[g_itemNames[i].name] = g_itemNames[i].id;

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
	list<StringTableEntry> lStringTable;
	list<StringPointerEntry> lStringPointers;
	list<comboRecord> lComboRecords;
	list<comboItemRecord> lItemRecords;
	for(XMLElement* elem = root->FirstChildElement("combo"); elem != NULL; elem = elem->NextSiblingElement("combo"))
	{
		comboRecord cr;
		int id;
		if(elem->QueryIntAttribute("id", &id) != XML_NO_ERROR)
		{
			cout << "Unable to get combo ID from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		int coinval;
		if(elem->QueryIntAttribute("coinvalue", &coinval) != XML_NO_ERROR)
		{
			cout << "Unable to get coin value from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		int stampval;
		if(elem->QueryIntAttribute("stampvalue", &stampval) != XML_NO_ERROR)
		{
			cout << "Unable to get stamp value from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		
		cr.id = id;
		cr.title.key = lStringTable.size();
		cr.value = coinval;
		cr.stampValue = stampval;
		cr.firstItemIdx = lItemRecords.size();
		cr.numItems = 0;
		//TODO cr.idStrTblIdx = from XML when available
		XMLElement* elem2 = elem->FirstChildElement("name");
		if(elem2 == NULL) continue;
		if(elem2->QueryUnsignedAttribute("strid", &cr.title.id) != XML_NO_ERROR)
		{
			cout << "Unable to get string id from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		
		//Read in combo name strings
		StringTableEntry ste;
		ste.pointerIndex = lStringPointers.size();
		ste.pointerCount = 0;
		for(XMLElement* elem3 = elem2->FirstChildElement("string"); elem3 != NULL; elem3 = elem3->NextSiblingElement("string"))
		{
			ste.pointerCount++;
			StringPointerEntry spe;
			//Get language ID for this string
			const char* lang = elem3->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to get string language from XML file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			spe.languageId = toLangID(s2ws(lang));
			spe.offset = lUTFData.size();
			
			//Get string data
			const char* cString = elem3->Attribute("data");
			if(cString == NULL)
			{
				cout << "Unable to get string data from XML file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			
			for(int i = 0; cString[i] != '\0'; i++)
				lUTFData.push_back(cString[i]);
			lUTFData.push_back('\0');
			
			lStringPointers.push_back(spe);
		}
		lStringTable.push_back(ste);
		
		//Read in combo items
		elem2 = elem->FirstChildElement("items");
		if(elem2 == NULL) continue;
		for(XMLElement* elem3 = elem2->FirstChildElement("item"); elem3 != NULL; elem3 = elem3->NextSiblingElement("item"))
		{
			cr.numItems++;
			comboItemRecord cir;
			//Get item ID
			const char* cName = elem3->Attribute("name");
			if(cName == NULL)
			{
				cout << "Unable to get item name from XML file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			cir.itemId = m_itemNames[cName];
			
			//Get texture coordinates
			const char* cPos = elem3->Attribute("texpos");
			if(cPos == NULL)
			{
				cout << "Unable to get item texpos from XML file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			istringstream iss(cPos);
			char cDiscard;
			iss >> cir.picTexX >> cDiscard >> cir.picTexY;
			
			const char* cSize = elem3->Attribute("texsize");
			if(cPos == NULL)
			{
				cout << "Unable to get item texsize from XML file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			istringstream iss2(cSize);
			iss2 >> cir.picTexW >> cDiscard >> cir.picTexH;
			
			lItemRecords.push_back(cir);
			
		}
		lComboRecords.push_back(cr);
	}
	delete doc;	//Done reading XML
	
	//Open our output file
	FILE* f = _wfopen(cFilename, TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(cFilename) << endl;
		return false;
	}
	
	//Write out combo database header
	comboHeader ch;
	ch.combos.count = lComboRecords.size();
	ch.combos.offset = sizeof(comboHeader);
	ch.items.count = lItemRecords.size();
	ch.items.offset = sizeof(comboHeader) + sizeof(comboRecord) * ch.combos.count;
	ch.stringTableBytes.count = sizeof(StringTableHeader) + sizeof(StringTableEntry) * lStringTable.size() + sizeof(StringPointerEntry) * lStringPointers.size() + lUTFData.size();
	ch.stringTableBytes.offset = sizeof(comboHeader) + sizeof(comboRecord) * ch.combos.count + sizeof(comboItemRecord) * ch.items.count;
	fwrite(&ch, 1, sizeof(comboHeader), f);
	
	//Write out combo records
	for(list<comboRecord>::iterator i = lComboRecords.begin(); i != lComboRecords.end(); i++)
		fwrite(&(*i), 1, sizeof(comboRecord), f);
	
	//Write out combo item records
	for(list<comboItemRecord>::iterator i = lItemRecords.begin(); i != lItemRecords.end(); i++)
		fwrite(&(*i), 1, sizeof(comboItemRecord), f);
		
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
















