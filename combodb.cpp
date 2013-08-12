#include "pakDataTypes.h"

bool comboDBToXML(const wchar_t* cFilename)
{
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
	vector<wchar_t> vStringList;
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
		vStringList.push_back(c);
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
			elem3->SetAttribute("data", ws2s(&(vStringList.data()[vStringPointerList[j].offset])).c_str());	//TODO: Why ASCII here?
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
				elem4->SetAttribute("id", vComboItems[j].itemId);	//TODO: Item name
				elem4->SetAttribute("texx", vComboItems[j].picTexX);
				elem4->SetAttribute("texy", vComboItems[j].picTexY);
				elem4->SetAttribute("texw", vComboItems[j].picTexW);
				elem4->SetAttribute("texh", vComboItems[j].picTexH);
				elem3->InsertEndChild(elem4);
			}
			elem->InsertEndChild(elem3);
		}
		root->InsertEndChild(elem);
	}
	//Write string table
	/*for(int i = 0; i < vStringTableList.size(); i++)
	{
		XMLElement* elem = doc->NewElement("text");
		for(int j = vStringTableList[i].pointerIndex; j < vStringTableList[i].pointerIndex + vStringTableList[i].pointerCount; j++)
		{
			XMLElement* elem2 = doc->NewElement("string");
			elem2->SetAttribute("lang", ws2s(toLangString(vStringPointerList[j].languageId)).c_str());
			elem2->SetAttribute("data", ws2s(&(vStringList.data()[vStringPointerList[j].offset])).c_str());
			elem->InsertEndChild(elem2);
		}
		doc->InsertEndChild(elem);
	}*/
	doc->InsertFirstChild(root);
	
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
}

bool XMLToComboDB(const wchar_t* cFilename)
{
}