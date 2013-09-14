#include "pakDataTypes.h"

//Default values for pages
#define DEFAULT_ALLOWSKIP				1
#define DEFAULT_LASTWORDSOUNDRESID		0
#define DEFAULT_MUSICOFFSET				0.000000
#define DEFAULT_SENDERPICEXPORTID		0
#define DEFAULT_SPECIALEVENTEXPORTID	0
#define DEFAULT_SPEEDSCALE				1.000000
#define DEFAULT_MUSICSOUNDRESID			0
//#define DEFAULT_WORDSSOUNDRESID		484346969

//Default values for letters
#define DEFAULT_ADDTRAYSLOTS				0
#define DEFAULT_ATTACHEDCATALOGID			0
#define DEFAULT_ATTACHEDITEMID				0
#define DEFAULT_ATTACHEDMONEYAMOUNT			0
#define DEFAULT_BLOCKNEXTCATALOG			0
//#define DEFAULT_BORDERANIMEXPORTID		3374510817
#define DEFAULT_BURNABLEAFTERREAD			1
#define DEFAULT_DELIVERAFTERWIN				0
#define DEFAULT_DELIVERABLE					1
#define DEFAULT_DEPALLCOMBO					0
#define DEFAULT_DEPALLITEMSUSED				0
#define DEFAULT_DEPALLSTARS					0
#define DEFAULT_DEPNOITEMS					0
#define DEFAULT_DEPENDSLETTERID				0
#define DEFAULT_JINGLELETTER				0
#define DEFAULT_POSTLETTERSPECIALEXPORTID	0
#define DEFAULT_REMOVEAFTERREAD				0
#define DEFAULT_REQCHILDWRONGLETTERID		0
#define DEFAULT_REQPARENTLETTERID			0
#define DEFAULT_SENDTIMESEC					5
#define DEFAULT_TYPE						BASIC_LETTER

//DEBUG Convert a string to uppercase
/*string stoupper( const string s )
{
  string result = s;
  for(unsigned int i = 0; i < s.size(); i++)
  {
	char c = s[i];
	if( (c >= 'a') && (c <= 'z') )
	{
		c -= 'a' - 'A';
		result[i] = c;
	}
  }
  
  return result;
}*/

bool letterToXML(wstring sFilename)
{
	FILE* f = _wfopen(sFilename.c_str(), TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(sFilename) << " for reading." << endl;
		return false;
	}
	
	//Read letter header
	letterHeader lh;
	if(fread(&lh, 1, sizeof(letterHeader), f) != sizeof(letterHeader))
	{
		cout << "Error: unable to read letterHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in letters
	list<letter> lLetters;
	for(int i = 0; i < lh.letters.count; i++)
	{
		letter l;
		if(fread(&l, 1, sizeof(letter), f) != sizeof(letter))
		{
			cout << "Error: unable to read letter " << i << " from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lLetters.push_back(l);
	}
	
	//Read in pages
	vector<letterPage> vPages;
	for(int i = 0; i < lh.pages.count; i++)
	{
		letterPage lp;
		if(fread(&lp, 1, sizeof(letterPage), f) != sizeof(letterPage))
		{
			cout << "Error: unable to read letterPage " << i << " from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		vPages.push_back(lp);
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
	
	
	//DEBUG: See how often each value occurs and see if there's a resonable default
	//map<string, list<string> > mPageOccurrences;
	//map<string, list<string> > mLetterOccurrences;
	
	//Done reading
	fclose(f);
	
	//Write out to XML
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("letters");
	
	for(list<letter>::iterator i = lLetters.begin(); i != lLetters.end(); i++)
	{
		XMLElement* letter = doc->NewElement("letter");
		
		//TODO: Letter defaults
		
		switch(i->type)
		{
			case DEFAULT_TYPE:
				//letter->SetAttribute("type", "basic");
				break;
				
			case REQUEST_LETTER:
				letter->SetAttribute("type", "request");
				break;
				
			case REQUESTFOLLOWUP_LETTER:
				letter->SetAttribute("type", "requestFollowUp");
				break;
				
			case ATTACHITEM_LETTER:
				letter->SetAttribute("type", "attachItem");
				break;
				
			case ATTACHMONEY_LETTER:
				letter->SetAttribute("type", "attachMoney");
				break;
				
			case ATTACHCATALOG_LETTER:
				letter->SetAttribute("type", "attachCatalog");
				break;
			
			default:
				cout << "Unknown letter type: " << i->type << endl;
				delete doc;
				return false;
		}
		
		//Set attributes, keeping default values in mind
		if(i->addTraySlots != DEFAULT_ADDTRAYSLOTS)
			letter->SetAttribute("addTraySlots", i->addTraySlots);
		if(i->attachedCatalogId != DEFAULT_ATTACHEDCATALOGID)
			letter->SetAttribute("attachedCatalogId", i->attachedCatalogId);
		if(i->attachedItemId != DEFAULT_ATTACHEDITEMID)
			letter->SetAttribute("attachedItemId", i->attachedItemId);
		if(i->attachedMoneyAmount != DEFAULT_ATTACHEDMONEYAMOUNT)
			letter->SetAttribute("attachedMoneyAmount", i->attachedMoneyAmount);
		if(i->blockNextCatalog != DEFAULT_BLOCKNEXTCATALOG)
			letter->SetAttribute("blockNextCatalog", i->blockNextCatalog);
		letter->SetAttribute("borderAnimExportId", i->borderAnimExportId);
		if(i->burnableAfterRead != DEFAULT_BURNABLEAFTERREAD)
			letter->SetAttribute("burnableAfterRead", i->burnableAfterRead);
		letter->SetAttribute("delayTimeSec", i->delayTimeSec);
		if(i->deliverAfterWin != DEFAULT_DELIVERAFTERWIN)
			letter->SetAttribute("deliverAfterWin", i->deliverAfterWin);
		if(i->deliverable != DEFAULT_DELIVERABLE)
			letter->SetAttribute("deliverable", i->deliverable);
		if(i->depAllCombo != DEFAULT_DEPALLCOMBO)
			letter->SetAttribute("depAllCombo", i->depAllCombo);
		if(i->depAllItemsUsed != DEFAULT_DEPALLITEMSUSED)
			letter->SetAttribute("depAllItemsUsed", i->depAllItemsUsed);
		if(i->depAllStars != DEFAULT_DEPALLSTARS)
			letter->SetAttribute("depAllStars", i->depAllStars);
		letter->SetAttribute("depCatalogId", i->depCatalogId);
		letter->SetAttribute("depCatalogItemCount", i->depCatalogItemCount);
		if(i->depNoItems != DEFAULT_DEPNOITEMS)
			letter->SetAttribute("depNoItems", i->depNoItems);
		if(i->dependsLetterId != DEFAULT_DEPENDSLETTERID)
			letter->SetAttribute("dependsLetterId", i->dependsLetterId);
		letter->SetAttribute("id", i->id);
		if(i->jingleLetter != DEFAULT_JINGLELETTER)
			letter->SetAttribute("jingleLetter", i->jingleLetter);
		if(i->postLetterSpecialExportId != DEFAULT_POSTLETTERSPECIALEXPORTID)
			letter->SetAttribute("postLetterSpecialExportId", i->postLetterSpecialExportId);
		if(i->removeAfterRead != DEFAULT_REMOVEAFTERREAD)
			letter->SetAttribute("removeAfterRead", i->removeAfterRead);
		if(i->reqChildWrongLetterId != DEFAULT_REQCHILDWRONGLETTERID)
			letter->SetAttribute("reqChildWrongLetterId", i->reqChildWrongLetterId);
		if(i->reqParentLetterId != DEFAULT_REQPARENTLETTERID)
			letter->SetAttribute("reqParentLetterId", i->reqParentLetterId);
		if(i->sendTimeSec != DEFAULT_SENDTIMESEC)
			letter->SetAttribute("sendTimeSec", i->sendTimeSec);

		//Requested items is an array; make it a child element
		if(i->requestedItemId[0] || i->requestedItemId[1] || i->requestedItemId[2])
		{
			XMLElement* requestedItems = doc->NewElement("requested");
			for(int j = 0; j < 3; j++)
			{
				if(i->requestedItemId[j])
				{
					XMLElement* item = doc->NewElement("item");
					item->SetAttribute("id", itemIDToName(i->requestedItemId[j]).c_str());	//TODO: Support new items
					requestedItems->InsertEndChild(item);
				}
			}
			letter->InsertEndChild(requestedItems);
		}
		
		//letterIdStrId is a string; handle differently
		XMLElement* name = doc->NewElement("name");
		for(int j = vStringTableList[i->letterIdStrId].pointerIndex; j < vStringTableList[i->letterIdStrId].pointerIndex + vStringTableList[i->letterIdStrId].pointerCount; j++)
		{
			XMLElement* string = doc->NewElement("string");
			string->SetAttribute("lang", ws2s(toLangString(vStringPointerList[j].languageId)).c_str());
			string->SetAttribute("data", &(vStringList.data()[vStringPointerList[j].offset]));
			name->InsertEndChild(string);
		}
		letter->InsertFirstChild(name);
		
		//Insert pages
		XMLElement* pages = doc->NewElement("pages");
		for(int j = i->firstPageIdx; j < i->firstPageIdx + i->numPages; j++)
		{
			XMLElement* page = doc->NewElement("page");
			
			//Add page element, taking defaults into account
			if(vPages[j].allowSkip != DEFAULT_ALLOWSKIP)
				page->SetAttribute("allowSkip", vPages[j].allowSkip);
			if(vPages[j].lastWordSoundResId != DEFAULT_LASTWORDSOUNDRESID)
				page->SetAttribute("lastWordSoundResId", vPages[j].lastWordSoundResId);
			if(vPages[j].musicOffset != DEFAULT_MUSICOFFSET)
				page->SetAttribute("musicOffset", vPages[j].musicOffset);
			if(vPages[j].musicSoundResId != DEFAULT_MUSICSOUNDRESID)
				page->SetAttribute("musicSoundResId", ws2s(getName(vPages[j].musicSoundResId)).c_str());
			if(vPages[j].pagePicTexResId)
			{
				page->SetAttribute("pagePicTexResId", ws2s(getName(vPages[j].pagePicTexResId)).c_str());
				page->SetAttribute("pagePicScale", vPages[j].pagePicScale);	//pagePicScale is always 1.0 if pagePicTexResId is 0
			}
			if(vPages[j].senderPicExportId != DEFAULT_SENDERPICEXPORTID)
				page->SetAttribute("senderPicExportId", vPages[j].senderPicExportId);	//TODO: Not a resource ID. What is this?
			if(vPages[j].specialEventExportId != DEFAULT_SPECIALEVENTEXPORTID)
				page->SetAttribute("specialEventExportId", vPages[j].specialEventExportId);
			if(vPages[j].speedScale != DEFAULT_SPEEDSCALE)
				page->SetAttribute("speedScale", vPages[j].speedScale);
			page->SetAttribute("wordsSoundResId", vPages[j].wordsSoundResId);	//TODO: Not a resource ID. What is this?
			
			//Add page text
			XMLElement* text = doc->NewElement("text");
			text->SetAttribute("strid", vPages[j].text.id);
			for(int k = vStringTableList[vPages[j].text.key].pointerIndex; k < vStringTableList[vPages[j].text.key].pointerIndex + vStringTableList[vPages[j].text.key].pointerCount; k++)
			{
				XMLElement* string = doc->NewElement("string");
				string->SetAttribute("lang", ws2s(toLangString(vStringPointerList[k].languageId)).c_str());
				string->SetAttribute("data", &(vStringList.data()[vStringPointerList[k].offset]));
				text->InsertEndChild(string);
			}
			page->InsertEndChild(text);
			pages->InsertEndChild(page);
			
			//DEBUG Now loop back through here and pull data from it all
			//for(const XMLAttribute* att = page->FirstAttribute(); att != NULL; att = att->Next())
			//	mPageOccurrences[att->Name()].push_back(att->Value());
		}
		letter->InsertEndChild(pages);
		
		//DEBUG Now loop back through here and pull data from it all
		//for(const XMLAttribute* att = letter->FirstAttribute(); att != NULL; att = att->Next())
		//	mLetterOccurrences[att->Name()].push_back(att->Value());
		
		root->InsertEndChild(letter);
	}
	
	doc->InsertFirstChild(root);
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
	
	//DEBUG: Find most common ocurrences
	/*ofstream ofile("map2.txt");
	ofstream ofMap("map.txt");
	ofstream ofCode("mapcode.txt");
	for(map<string, list<string> >::iterator i = mLetterOccurrences.begin(); i != mLetterOccurrences.end(); i++)
	{
		//ofile << i->first << " ";
		map<string, unsigned int> mPer;
		
		//i->second.sort();
		for(list<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
		{
			if(mPer.count(*j))
				mPer[*j]++;
			else
				mPer[*j] = 1;
		}
		string sHighest = "";
		unsigned int iHighest = 0;
		for(map<string, unsigned int>::iterator j = mPer.begin(); j != mPer.end(); j++)
		{
			if(j->second > iHighest)
			{
				iHighest = j->second;
				sHighest = j->first;
			}
		}
		if(iHighest > i->second.size() / 2)	//Has to be over half to be a reasonable default
		{
			ofile << i->first << " had the default value " << sHighest << ", which occurred " << iHighest << " times." << endl;
			ofMap << "#define DEFAULT_" << stoupper(i->first) << "\t" << sHighest << endl;
			ofCode << "\tif(j->" << i->first << " != DEFAULT_" << stoupper(i->first) << ")" << endl
				   << "\t\telem3->SetAttribute(\"" << i->first << "\", j->" << i->first << ");" << endl;
		}
		else
		{
			ofCode << "\telem3->SetAttribute(\"" << i->first << "\", j->" << i->first << ");" << endl;
		}
		//ofile << *j << " ";
		//ofile << endl << endl;
	}
	ofile.close();
	ofMap.close();
	ofCode.close();*/
	
	return true;
}

bool XMLToLetter(wstring sFilename)
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
	list<letter> lLetters;
	list<letterPage> lPages;
	for(XMLElement* letterelem = root->FirstChildElement("letter"); letterelem != NULL; letterelem = letterelem->NextSiblingElement("letter"))
	{
		letter l;
		
		l.type = DEFAULT_TYPE;
		l.addTraySlots = DEFAULT_ADDTRAYSLOTS;
		l.attachedCatalogId = DEFAULT_ATTACHEDCATALOGID;
		l.attachedItemId = DEFAULT_ATTACHEDITEMID;
		l.attachedMoneyAmount = DEFAULT_ATTACHEDMONEYAMOUNT;
		l.blockNextCatalog = DEFAULT_BLOCKNEXTCATALOG;
		l.burnableAfterRead = DEFAULT_BURNABLEAFTERREAD;
		l.deliverAfterWin = DEFAULT_DELIVERAFTERWIN;
		l.deliverable = DEFAULT_DELIVERABLE;
		l.depAllCombo = DEFAULT_DEPALLCOMBO;
		l.depAllItemsUsed = DEFAULT_DEPALLITEMSUSED;
		l.depAllStars = DEFAULT_DEPALLSTARS;
		l.depNoItems = DEFAULT_DEPNOITEMS;
		l.dependsLetterId = DEFAULT_DEPENDSLETTERID;
		l.jingleLetter = DEFAULT_JINGLELETTER;
		l.postLetterSpecialExportId = DEFAULT_POSTLETTERSPECIALEXPORTID;
		l.removeAfterRead = DEFAULT_REMOVEAFTERREAD;
		l.reqChildWrongLetterId = DEFAULT_REQCHILDWRONGLETTERID;
		l.reqParentLetterId = DEFAULT_REQPARENTLETTERID;
		l.sendTimeSec = DEFAULT_SENDTIMESEC;
			
		//Required fields
		if(letterelem->QueryUnsignedAttribute("depCatalogId", &l.depCatalogId) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read letter depCatalogId from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(letterelem->QueryIntAttribute("depCatalogItemCount", &l.depCatalogItemCount) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read letter depCatalogItemCount from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(letterelem->QueryIntAttribute("delayTimeSec", &l.delayTimeSec) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read letter delayTimeSec from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(letterelem->QueryUnsignedAttribute("borderAnimExportId", &l.borderAnimExportId) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read letter borderAnimExportId from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		if(letterelem->QueryUnsignedAttribute("id", &l.id) != XML_NO_ERROR)
		{
			cout << "Error: Unable to read letter id from XML file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		
		//Read in optional attributes
		const char* cType = letterelem->Attribute("type");
		if(cType != NULL)
		{
			string s = cType;
			if(s == "basic")
				l.type = DEFAULT_TYPE;
			else if(s == "request")
				l.type = REQUEST_LETTER;
			else if(s == "requestFollowUp")
				l.type = REQUESTFOLLOWUP_LETTER;
			else if(s == "attachItem")
				l.type = ATTACHITEM_LETTER;
			else if(s == "attachMoney")
				l.type = ATTACHMONEY_LETTER;
			else if(s == "attachCatalog")
				l.type = ATTACHCATALOG_LETTER;
			else
			{
				cout << "Error: unknown letter type: " << s << " in file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
		}
		letterelem->QueryUnsignedAttribute("dependsLetterId", &l.dependsLetterId);
		letterelem->QueryIntAttribute("sendTimeSec", &l.sendTimeSec);
		letterelem->QueryUnsignedAttribute("postLetterSpecialExportId", &l.postLetterSpecialExportId);
		letterelem->QueryIntAttribute("depNoItems", &l.depNoItems);
		letterelem->QueryIntAttribute("addTraySlots", &l.addTraySlots);
		letterelem->QueryIntAttribute("deliverable", &l.deliverable);
		letterelem->QueryIntAttribute("removeAfterRead", &l.removeAfterRead);
		letterelem->QueryIntAttribute("burnableAfterRead", &l.burnableAfterRead);
		letterelem->QueryIntAttribute("depAllStars", &l.depAllStars);
		letterelem->QueryIntAttribute("depAllCombo", &l.depAllCombo);
		letterelem->QueryIntAttribute("depAllItemsUsed", &l.depAllItemsUsed);
		letterelem->QueryIntAttribute("jingleLetter", &l.jingleLetter);
		letterelem->QueryIntAttribute("blockNextCatalog", &l.blockNextCatalog);
		letterelem->QueryIntAttribute("deliverAfterWin", &l.deliverAfterWin);
		letterelem->QueryIntAttribute("attachedMoneyAmount", &l.attachedMoneyAmount);
		letterelem->QueryUnsignedAttribute("reqChildWrongLetterId", &l.reqChildWrongLetterId);
		letterelem->QueryUnsignedAttribute("reqParentLetterId", &l.reqParentLetterId);
		letterelem->QueryUnsignedAttribute("attachedItemId", &l.attachedItemId);
		letterelem->QueryUnsignedAttribute("attachedCatalogId", &l.attachedCatalogId);
		
		//Read requested item id's
		for(int j = 0; j < 3; j++)
			l.requestedItemId[j] = 0;
		XMLElement* requested = letterelem->FirstChildElement("requested");
		if(requested != NULL)	//Don't really care if this is even here or not
		{
			int iTotalItems = 0;
			for(XMLElement* item = requested->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
			{
				if(++iTotalItems > 3)
				{
					cout << "Warning: Only up to 3 requestedItemIds are allowed per letter. Ignoring extra item number " << iTotalItems << endl;
					continue;	//Skip, printing warning multiple times if there are more
				}
				
				const char* cID = item->Attribute("id");
				if(cID == NULL)
				{
					cout << "Error reading requested item ID from file " << ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				l.requestedItemId[iTotalItems-1] = itemNameToID(cID);
			}
		}
		
		//Read letter name string
		StringTableEntry nameEntry;
		nameEntry.pointerIndex = lStringPointers.size();
		nameEntry.pointerCount = 0;
		l.letterIdStrId = lStringTable.size();
		XMLElement* namestr = letterelem->FirstChildElement("name");	//Get ID string
		if(namestr == NULL)
		{
			cout << "Error reading letter name string from file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* namestring = namestr->FirstChildElement("string"); namestring != NULL; namestring = namestring->NextSiblingElement("string"))
		{
			StringPointerEntry spe;
			const char* lang = namestring->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to read letter id string's language from XML file " <<  ws2s(sXMLFile) << endl;
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
				cout << "Unable to read letter id string's data from XML file " <<  ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			for(int j = 0; j < strlen(data); j++)
				lUTFData.push_back(data[j]);
			lUTFData.push_back('\0');	//Be sure to append null character, as well
		}
		lStringTable.push_back(nameEntry);
		
		//Read children pages
		l.firstPageIdx = lPages.size();
		l.numPages = 0;
		XMLElement* pages = letterelem->FirstChildElement("pages");
		if(pages == NULL)
		{
			cout << "Error reading letter pages from file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* page = pages->FirstChildElement("page"); page != NULL; page = page->NextSiblingElement("page"))
		{
			letterPage lp;
			
			//Set default values
			lp.allowSkip = DEFAULT_ALLOWSKIP;
			lp.lastWordSoundResId = DEFAULT_LASTWORDSOUNDRESID;
			lp.musicOffset = DEFAULT_MUSICOFFSET;
			lp.musicSoundResId = DEFAULT_MUSICSOUNDRESID;
			lp.senderPicExportId = DEFAULT_SENDERPICEXPORTID;
			lp.specialEventExportId = DEFAULT_SPECIALEVENTEXPORTID;
			lp.speedScale = DEFAULT_SPEEDSCALE;
			lp.pagePicScale = 1.0;
			lp.pagePicTexResId = 0;
			
			//Query necessary values
			if(page->QueryUnsignedAttribute("wordsSoundResId", &lp.wordsSoundResId) != XML_NO_ERROR)
			{
				cout << "Error reading page wordsSoundResId from file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			
			//Query everything else, ignoring failure
			page->QueryUnsignedAttribute("senderPicExportId", &lp.senderPicExportId);
			page->QueryUnsignedAttribute("specialEventExportId", &lp.specialEventExportId);
			page->QueryUnsignedAttribute("lastWordSoundResId", &lp.lastWordSoundResId);
			page->QueryFloatAttribute("pagePicScale", &lp.pagePicScale);
			page->QueryFloatAttribute("musicOffset", &lp.musicOffset);
			page->QueryFloatAttribute("speedScale", &lp.speedScale);
			page->QueryIntAttribute("allowSkip", &lp.allowSkip);
			const char* pageTex = page->Attribute("pagePicTexResId");
			if(pageTex != NULL)
				lp.pagePicTexResId = getResID(s2ws(pageTex).c_str());
			const char* musicID = page->Attribute("musicSoundResId");
			if(musicID != NULL)
				lp.musicSoundResId = getResID(s2ws(musicID).c_str());

			StringTableEntry pageEntry;
			pageEntry.pointerIndex = lStringPointers.size();
			pageEntry.pointerCount = 0;
			lp.text.key = lStringTable.size();
			XMLElement* textstr = page->FirstChildElement("text");	//Get ID string
			if(textstr == NULL)
			{
				cout << "Error reading page text string from file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			if(textstr->QueryUnsignedAttribute("strid", &lp.text.id) != XML_NO_ERROR)
			{
				cout << "Error reading page text string's strid from file " << ws2s(sXMLFile) << endl;
				delete doc;
				return false;
			}
			for(XMLElement* namestring = textstr->FirstChildElement("string"); namestring != NULL; namestring = namestring->NextSiblingElement("string"))
			{
				StringPointerEntry spe;
				const char* lang = namestring->Attribute("lang");
				if(lang == NULL)
				{
					cout << "Unable to read page text string's language from XML file " <<  ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				spe.languageId = toLangID(s2ws(lang));
				spe.offset = lUTFData.size();
				lStringPointers.push_back(spe);
				pageEntry.pointerCount++;
				
				//Read in string data
				const char* data = namestring->Attribute("data");
				if(data == NULL)
				{
					cout << "Unable to read page text string's data from XML file " <<  ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				for(int j = 0; j < strlen(data); j++)
					lUTFData.push_back(data[j]);
				lUTFData.push_back('\0');	//Be sure to append null character, as well
			}
			lStringTable.push_back(pageEntry);
			
			
			lPages.push_back(lp);
			l.numPages++;
		}
		
		lLetters.push_back(l);
		
	}
	//Done with XML file
	delete doc;
	
	//Open our output file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(sFilename) << endl;
		return false;
	}
	
	//Write out letter database header
	letterHeader lh;
	lh.letters.count = lLetters.size();
	lh.letters.offset = sizeof(letterHeader);
	lh.pages.count = lPages.size();
	lh.pages.offset = lh.letters.offset + lh.letters.count * sizeof(letter);
	lh.stringTableBytes.count = sizeof(StringTableHeader) + lStringTable.size() * sizeof(StringTableEntry) + lStringPointers.size() * sizeof(StringPointerEntry) + lUTFData.size();
	lh.stringTableBytes.offset = lh.pages.offset + lh.pages.count * sizeof(letterPage);
	fwrite(&lh, 1, sizeof(letterHeader), f);
	
	//Write out letter records
	for(list<letter>::iterator i = lLetters.begin(); i != lLetters.end(); i++)
		fwrite(&(*i), 1, sizeof(letter), f);
	
	//Write out letter pages
	for(list<letterPage>::iterator i = lPages.begin(); i != lPages.end(); i++)
		fwrite(&(*i), 1, sizeof(letterPage), f);
	
	//Write out string table header
	StringTableHeader sth;
	sth.numStrings = lStringTable.size();
	sth.numPointers = lStringPointers.size();
	fwrite(&sth, 1, sizeof(StringTableHeader), f);
	
	//Write out string table strings
	for(list<StringTableEntry>::iterator i = lStringTable.begin(); i != lStringTable.end(); i++)
		fwrite(&(*i), 1, sizeof(StringTableEntry), f);
		
	//Write out string table pointers
	for(list<StringPointerEntry>::iterator i = lStringPointers.begin(); i != lStringPointers.end(); i++)
		fwrite(&(*i), 1, sizeof(StringPointerEntry), f);
	
	//Write out strings
	for(list<char>::iterator i = lUTFData.begin(); i != lUTFData.end(); i++)
		fwrite(&(*i), 1, 1, f);
		
	fclose(f);
	
	return true;
}













