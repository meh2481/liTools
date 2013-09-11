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
		XMLElement* requestedItems = doc->NewElement("requested");
		for(int j = 0; j < 3; j++)
		{
			if(i->requestedItemId[j])
			{
				XMLElement* item = doc->NewElement("item");
				item->SetAttribute("id", i->requestedItemId[j]);
				requestedItems->InsertEndChild(item);
			}
		}
		letter->InsertEndChild(requestedItems);
		
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
	//TODO
	return true;
}