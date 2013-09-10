#include "pakDataTypes.h"
#include <sstream>

#define TYPE_POLYGON	0x04
#define TYPE_CIRCLE		0x02

#define SPLIT_XML_FILES

//Defaults for item data XML
#define DEFAULT_ABSPOSITION				0
#define DEFAULT_ALLOWDIRECTIONALLIGHT	1
#define DEFAULT_ALLOWEXPLODESTREAKS		1
#define DEFAULT_ANIMTHRESHOLD			0
#define DEFAULT_BURNSLOWSANIM			1
#define DEFAULT_CANGETPLAGUE			1
#define DEFAULT_COLLIDEENVIRONMENT		1
#define DEFAULT_COLLIDEITEMS			1
#define DEFAULT_COSTSTAMPS				0
//#define DEFAULT_DESCKEY					2
#define DEFAULT_ENABLEFREEZEPOSTANIM	1
#define DEFAULT_ENABLEHFLIP				1
#define DEFAULT_FLOORWALLSHADOW			1
#define DEFAULT_FREEZEONCOLLIDE			0
#define DEFAULT_ILLUMINATE				0
#define DEFAULT_INITIALBURNEXPORTID		0
#define DEFAULT_INITIALBURNPERGROUP		0
#define DEFAULT_INSTASHDOESSPLAT		0
#define DEFAULT_INSTASHSOUNDRESID		0
#define DEFAULT_INSTANTEFFECTS			0
//#define DEFAULT_ITEMIDSTRID				0
#define DEFAULT_MODXAMPMAX				0
#define DEFAULT_MODXAMPMIN				0
#define DEFAULT_MODXFREQMAX				1
#define DEFAULT_MODXFREQMIN				1
#define DEFAULT_MODXPHASEMAX			0
#define DEFAULT_MODXPHASEMIN			0
#define DEFAULT_MODXSPEEDMAX			0
#define DEFAULT_MODXSPEEDMIN			0
#define DEFAULT_MODYAMPMAX				0
#define DEFAULT_MODYAMPMIN				0
#define DEFAULT_MODYFREQMAX				1
#define DEFAULT_MODYFREQMIN				1
#define DEFAULT_MODYPHASEMAX			0
#define DEFAULT_MODYPHASEMIN			0
#define DEFAULT_MODYSPEEDMAX			0
#define DEFAULT_MODYSPEEDMIN			0
#define DEFAULT_MONEYITEM				0
#define DEFAULT_MOTORTHRESHOLD			-1
#define DEFAULT_MOUSEGRABSOUNDRESID		0
#define DEFAULT_MOUSEGRABBABLE			1
//#define DEFAULT_NAMEKEY					1
#define DEFAULT_ORBITALGRAVITY			0
#define DEFAULT_PLAGUEONCOLLIDE			0
#define DEFAULT_POPSCOINS				1
#define DEFAULT_QUANTITY				1
#define DEFAULT_SCALEVARIANCE			0
#define DEFAULT_SPAWNLIMITBURNEXPORTID	0
#define DEFAULT_SPLITJUMPLASTFRAME		0
#define DEFAULT_UNIQUEIGNITESOUNDRESID	0
#define DEFAULT_UNLISTED				0
#define DEFAULT_VALUESTAMPS				0

//Default values for bone data XML
#define DEFAULT_APPLYGRAVITYENUMVALID					4108830781u
#define DEFAULT_ASHSPLITTIMEBASEENUMVALID				1227225697
#define DEFAULT_ASHSPLITTIMEVARENUMVALID				1227225697
#define DEFAULT_AUTOROTATEUPRIGHTENUMVALID				1086434539
#define DEFAULT_BEHAVIOR								1
#define DEFAULT_BURNGRIDSIZE							10
#define DEFAULT_CONNECTEDGROUPIDX						-1
#define DEFAULT_DECAYPARTICLESENUMVALID					1227225697
#define DEFAULT_EXPLODEIGNITEPIECESENUMVALID			1086434539
#define DEFAULT_EXPLODEIGNOREBURNTRIGGERENUMVALID		1086434539
#define DEFAULT_IGNITEPARTICLESENUMVALID				1227225697
#define DEFAULT_INSTASHONCOLLIDEENUMVALID				1086434539
//#define DEFAULT_NUMPARTS								1
//#define DEFAULT_NUMRGNCELLS								0
#define DEFAULT_POSTEXPLODEASHBREAKMINACCELENUMVALID	1227225697
#define DEFAULT_POSTEXPLODESPLITTIMEBASEENUMVALID		1227225697
#define DEFAULT_POSTEXPLODESPLITTIMEVARENUMVALID		1227225697
#define DEFAULT_SHATTEREXPDOCAMSHAKEENUMVALID			4108830781u
#define DEFAULT_SHATTEREXPEFFECTENUMVALID				1227225697
#define DEFAULT_SHATTEREXPFIREAMOUNTENUMVALID			1227225697
#define DEFAULT_SHATTEREXPFIRESPEEDENUMVALID			1227225697
#define DEFAULT_SHATTEREXPFORCEENUMVALID				1227225697
#define DEFAULT_SHATTEREXPRADIUSENUMVALID				1227225697
#define DEFAULT_SHATTEREXPSOUNDENUMVALID				1227225697
#define DEFAULT_SHATTEREXPTIMEFACTORENUMVALID			1227225697
#define DEFAULT_SHATTEREXPTIMEHOLDDOWNENUMVALID			1227225697
#define DEFAULT_SHATTEREXPTIMERAMPDOWNENUMVALID			1227225697
#define DEFAULT_SHATTEREXPTIMERAMPUPENUMVALID			1227225697
#define DEFAULT_SMEARAMOUNTENUMVALID					1227225697
#define DEFAULT_SPLATPARTICLESENUMVALID					1227225697
#define DEFAULT_SPLITBRITTLEENUMVALID					1227225697
#define DEFAULT_SPLITDESPAWNEFFECTENUMVALID				1227225697
#define DEFAULT_SPLITEFFECTENUMVALID					1227225697
#define DEFAULT_SPLITSFXLARGEENUMVALID					1227225697
#define DEFAULT_SPLITSFXMEDIUMENUMVALID					1227225697
#define DEFAULT_SPLITSFXSMALLENUMVALID					1227225697
#define DEFAULT_SPLITTHRESHOLDENUMVALID					1227225697

string vec2ToString(vec2 v)
{
	ostringstream oss;
	oss << v.x << ", " << v.y;
	return oss.str();
}

vec2 stringToVec2(string s)
{
	vec2 v;
	istringstream iss(s);
	char cDiscard;
	iss >> v.x >> cDiscard >> v.y;
	return v;
}

wstring toLangString(u32 languageId)	//Convert a language ID to a string (for example, 0x656E becomes "en")
{
	wstring s;
	s.push_back(languageId >> 8);
	s.push_back(languageId & 0xFF);
	return s;
}

u32 toLangID(wstring languageString)	//Convert a language string to an ID number (for example, "en" becomes 0x656E)
{
	string s = ws2s(languageString);
	if(s.size() < 2) return 0;
	u32 result;
	result = s[1];
	result += ((u32)(s[0])) << 8;
	return result;
}

/*wstring getNameFromAnim(wstring sAnimName)
{
	sAnimName.erase(sAnimName.rfind(TEXT(".anim.xml")));	//Erase the ending off the string
	size_t start = sAnimName.find_last_of('/') + 1;	//Find last forward slash
	sAnimName.erase(0,start);						//Erase everything up to and including this last slash
	return sAnimName;								//Done
}*/

void makeFolder(wstring sFilename)
{
	size_t pos = sFilename.find_last_of(L'/');
	if(pos != wstring::npos)
		sFilename = sFilename.substr(0,pos);
	sFilename = TEXT("./") + sFilename;
	ttvfs::CreateDirRec(ws2s(sFilename).c_str());	//I'm pretty sure this is threadsafe
}

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
#include <iomanip>
bool itemManifestToXML(const wchar_t* cFilename)
{
	//Open the file
	FILE* f = _wfopen(cFilename, TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(cFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in the manifest
	itemManifestHeader imh;
	if(fread(&imh, 1, sizeof(itemManifestHeader), f) != sizeof(itemManifestHeader))
	{
		cout << "Error: unable to read itemManifestHeader from file " << ws2s(cFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in the itemManifestRecords
	list<itemManifestRecord> lManifestRecords;
	map<u32, wstring> mItemNames;
	fseek(f, imh.itemsManifest.offset, SEEK_SET);
	for(int i = 0; i < imh.itemsManifest.count; i++)
	{
		itemManifestRecord imr;
		if(fread(&imr, 1, sizeof(itemManifestRecord), f) != sizeof(itemManifestRecord))
		{
			cout << "Error: unable to read itemManifestRecord from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		lManifestRecords.push_back(imr);
		mItemNames[imr.itemId] = s2ws(itemIDToName(imr.itemId));//getNameFromAnim(getName(imr.animResId));
	}
	
	//Read in the normalDependencies
	vector<normalDependency> vNormalDependencies;
	fseek(f, imh.normalDeps.offset, SEEK_SET);
	for(int i = 0; i < imh.normalDeps.count; i++)
	{
		normalDependency nd;
		if(fread(&nd, 1, sizeof(normalDependency), f) != sizeof(normalDependency))
		{
			cout << "Error: unable to read normalDependency from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vNormalDependencies.push_back(nd);
	}
	
	//Read in the soundDependencies
	vector<soundDependency> vSoundDependencies;
	fseek(f, imh.soundDeps.offset, SEEK_SET);
	for(int i = 0; i < imh.soundDeps.count; i++)
	{
		soundDependency sd;
		if(fread(&sd, 1, sizeof(soundDependency), f) != sizeof(soundDependency))
		{
			cout << "Error: unable to read soundDependency from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vSoundDependencies.push_back(sd);
	}
	
	//Read in the effectDependencies
	vector<effectDependency> vEffectDependencies;
	fseek(f, imh.effectDeps.offset, SEEK_SET);
	for(int i = 0; i < imh.effectDeps.count; i++)
	{
		effectDependency ed;
		if(fread(&ed, 1, sizeof(effectDependency), f) != sizeof(effectDependency))
		{
			cout << "Error: unable to read effectDependency from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vEffectDependencies.push_back(ed);
	}
	
	//Read in the itemDependencies
	vector<itemDependency> vItemDependencies;
	fseek(f, imh.itemDeps.offset, SEEK_SET);
	for(int i = 0; i < imh.itemDeps.count; i++)
	{
		itemDependency id;
		if(fread(&id, 1, sizeof(itemDependency), f) != sizeof(itemDependency))
		{
			cout << "Error: unable to read itemDependency from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vItemDependencies.push_back(id);
	}
	
	//Read in the item data
	vector<itemDataHeader> vItemDataHeaders;
	vector< list<skelsRecord> > vSkeletonRecords;
	vector< vector<jointRecord> > vJointRecords;
	vector< vector<boneRecord> > vBoneRecords;
	vector< vector<boneShapeRecord> > vBoneShapes;
	vector< vector<bonePartRecord> > vBoneParts;
	vector< vector<i32> > vBonePartTreeValues;
	vector< vector<boneGridCellMappingRegion> > vGridCellMappings;
	vector< vector<StringTableEntry> > vStringTableEntries;
	vector< vector<StringPointerEntry> > vStringPointerEntries;
	vector< vector<char> > vStrings;
	vector< vector<byte> > vBurnGrid;
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		fseek(f, imh.itemsBinDataBytes.offset + i->binDataOffsetBytes, SEEK_SET);	//Seek to this position to read
		//Read in header
		itemDataHeader idh;
		if(fread(&idh, 1, sizeof(itemDataHeader), f) != sizeof(itemDataHeader))
		{
			cout << "Error: Unable to read itemDataHeader from file " << ws2s(cFilename) << endl;
			fclose(f); 
			return false;
		}
		vItemDataHeaders.push_back(idh);
		
		//Read in skeleton records for this item data header
		list<skelsRecord> srl;
		for(int j = 0; j < idh.skels.count; j++)
		{
			skelsRecord sr;
			if(fread(&sr, 1, sizeof(skelsRecord), f) != sizeof(skelsRecord))
			{
				cout << "Error: Unable to read skelsRecord from file " << ws2s(cFilename) << ": " << j << ": " << ftell(f) << endl;
				fclose(f);
				return false;
			}
			srl.push_back(sr);
		}
		vSkeletonRecords.push_back(srl);
		
		//Read in joint records for this item data header
		vector<jointRecord> jrv;
		for(int j = 0; j < idh.joints.count; j++)
		{
			jointRecord jr;
			if(fread(&jr, 1, sizeof(jointRecord), f) != sizeof(jointRecord))
			{
				cout << "Error: Unable to read jointRecord from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
			}
			jrv.push_back(jr);
		}
		vJointRecords.push_back(jrv);
		
		//Read in bone records for this item data header
		vector<boneRecord> brv;
		for(int j = 0; j < idh.bones.count; j++)
		{
			boneRecord br;
			if(fread(&br, 1, sizeof(boneRecord), f) != sizeof(boneRecord))
			{
				cout << "Error: Unable to read boneRecord from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			brv.push_back(br);
		}
		vBoneRecords.push_back(brv);
		
		//Read in bone shape records for this item data header
		vector<boneShapeRecord> bsrv;
		for(int j = 0; j < idh.boneShapes.count; j++)
		{
			boneShapeRecord bsr;
			if(fread(&bsr, 1, sizeof(boneShapeRecord), f) != sizeof(boneShapeRecord))
			{
				cout << "Error: Unable to read boneShapeRecord from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			bsrv.push_back(bsr);
		}
		vBoneShapes.push_back(bsrv);
		
		//Read in bone part records
		vector<bonePartRecord> vbpr;
		for(int j = 0; j < idh.boneParts.count; j++)
		{
			bonePartRecord bpr;
			if(fread(&bpr, 1, sizeof(bonePartRecord), f) != sizeof(bonePartRecord))
			{
				cout << "Error: Unable to read bonePartRecord from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			vbpr.push_back(bpr);
		}
		vBoneParts.push_back(vbpr);
		
		//Read in bone tree values
		vector<i32> vbtv;
		for(int j = 0; j < idh.bonePartTreeVals.count; j++)
		{
			i32 boneTreeVal;
			if(fread(&boneTreeVal, 1, sizeof(i32), f) != sizeof(i32))
			{
				cout << "Error: Unable to read boneTreeVal from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			vbtv.push_back(boneTreeVal);
		}
		vBonePartTreeValues.push_back(vbtv);
		
		//Read in bone grid cell mapping regions
		vector<boneGridCellMappingRegion> vbgcmr;
		for(int j = 0; j < idh.rgnCells.count; j++)
		{
			boneGridCellMappingRegion bgcmr;
			if(fread(&bgcmr, 1, sizeof(boneGridCellMappingRegion), f) != sizeof(boneGridCellMappingRegion))
			{
				cout << "Error: Unable to read boneGridCellMappingRegion from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			vbgcmr.push_back(bgcmr);
		}
		vGridCellMappings.push_back(vbgcmr);
		
		//Read in string table-----------------------------------------------------------
		int iStringSize = 0;
		//Read in string table header
		StringTableHeader sth;
		if(fread((void*)&sth, 1, sizeof(StringTableHeader), f) != sizeof(StringTableHeader))
		{
			cout << "Error: Unable to read StringTableHeader from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		iStringSize += sizeof(StringTableHeader);
	
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
		vStringTableEntries.push_back(vStringTableList);
		iStringSize += sizeof(StringTableEntry) * sth.numStrings;
	
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
		vStringPointerEntries.push_back(vStringPointerList);
		iStringSize += sizeof(StringPointerEntry) * sth.numPointers;
	
		//Now read in the strings until we hit the end of where we're supposed to be
		while(true)
		{
			int c = fgetc(f);
			if(c == '\\')	//Change all backslashes to forward slashes. Tsk, tsk, Allan.
				c = '/';
			vStringList.push_back(c);
			if(++iStringSize == idh.stringTableBytes.count)
				break;	//Done here
		}
		vStrings.push_back(vStringList);
		//-----------------------------------------------------
		
		//Read in burn grid
		vector<byte> vbg;
		for(int j = 0; j < idh.burnGridUsedDataBytes.count; j++)
		{
			byte cell;
			if(fread(&cell, 1, 1, f) != 1)
			{
				cout << "Error: Unable to read grid cell from file " << ws2s(cFilename) << endl;
				fclose(f);
				return false;
 			}
			vbg.push_back(cell);
		}
		vBurnGrid.push_back(vbg);
	}
	
	fclose(f);
	
	//Now, open up the XML file and put all the data into it
	#ifndef SPLIT_XML_FILES
	wstring sFilename = cFilename;
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("itemmanifest");	//Create the root element
	#else
	wstring sFilename = cFilename;
	sFilename += TEXT(".xml");
	XMLDocument* itemmanifest = new XMLDocument;
	XMLElement* itemroot = itemmanifest->NewElement("items");
	#endif
	int iCurItemData = 0;
	
	//DEBUG: See how often each value occurs and see if there's a resonable default
	//map<string, list<string> > mOccurrences;
	//
	//ofstream oHash("hash2.txt");
	
	//DEBUG: Write out item name and ID, for easy conversion back-and-forth
	/*ofstream onames("itemnames.h");
	onames << "//Itemmanifest item names" << endl << endl
		   << "#ifndef ITEMNAMES_H" << endl << "#define ITEMNAMES_H" << endl << endl
		   << "#define NUM_ITEMNAMES " << lManifestRecords.size() << endl << endl
		   << "typedef struct" << endl << "{" << endl << "    u32 id;" << endl << "    const char* name;" << endl
		   << "} itemName;" << endl << endl
		   << "const itemName g_itemNames[NUM_ITEMNAMES] = {" << endl;*/
	
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		#ifdef SPLIT_XML_FILES
		XMLDocument* doc = new XMLDocument;
		wstring sXMLFilename = TEXT("data/items/");
		sXMLFilename += mItemNames[i->itemId];
		sXMLFilename += TEXT("/");
		sXMLFilename += mItemNames[i->itemId];
		sXMLFilename += TEXT(".xml");
		XMLElement* file = itemmanifest->NewElement("item");
		file->SetAttribute("path", ws2s(sXMLFilename).c_str());
		itemroot->InsertEndChild(file);
		#endif
		XMLElement* elem = doc->NewElement("itemrecord");
		//elem->SetAttribute("name", ws2s(mItemNames[i->itemId]).c_str());
		elem->SetAttribute("id", i->itemId);
		//DEBUG
		//oHash << "id: " << i->itemId << ", filename: " << mItemNames[i->itemId].c_str() 
		//	  << ", hashed filename: " << hash(mItemNames[i->itemId]) << endl;
		//if(i->itemId == hash(mItemNames[i->itemId]))
		//	oHash << "Hash worked!" << endl;
		//else
		//	oHash << "Hash failed." << endl;
		XMLElement* elem2 = doc->NewElement("animresid");
		elem2->SetAttribute("filename", ws2s(getName(i->animResId)).c_str());
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("recentlymodifiedrank");	//TODO Ignore this
		elem2->SetAttribute("value", i->recentlyModifiedRank);
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("coloritemicon");
		elem2->SetAttribute("filename", ws2s(getName(i->catalogIconColorItemTexResId)).c_str());
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("colorbgicon");
		elem2->SetAttribute("filename", ws2s(getName(i->catalogIconColorBGTexResId)).c_str());
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("greybgicon");
		elem2->SetAttribute("filename", ws2s(getName(i->catalogIconGreyBGTexResId)).c_str());
		elem->InsertEndChild(elem2);
		//Now insert dependencies for this item
		elem2 = doc->NewElement("depends");
		for(int j = i->firstNormalDepends; j < i->firstNormalDepends + i->numNormalDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("normal");
			elem3->SetAttribute("filename", ws2s(getName(vNormalDependencies[j].normalTexResId)).c_str());
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstSoundDepends; j < i->firstSoundDepends + i->numSoundDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("sound");
			elem3->SetAttribute("filename", ws2s(getSoundName(vSoundDependencies[j].soundResId)).c_str());
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstEffectDepends; j < i->firstEffectDepends + i->numEffectDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("effect");
			elem3->SetAttribute("filename", ws2s(getName(vEffectDependencies[j].effectResId)).c_str());
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstItemDepends; j < i->firstItemDepends + i->numItemDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("item");
			elem3->SetAttribute("id", ws2s(mItemNames[vItemDependencies[j].itemResId]).c_str());
			elem2->InsertEndChild(elem3);
		}
		elem->InsertEndChild(elem2);
		
		//Now deal with item data, only writing each if it differs from the default
		elem2 = doc->NewElement("itemdata");
		if(vItemDataHeaders[iCurItemData].absPosition != DEFAULT_ABSPOSITION)
			elem2->SetAttribute("absPosition", vItemDataHeaders[iCurItemData].absPosition);
		if(vItemDataHeaders[iCurItemData].allowDirectionalLight != DEFAULT_ALLOWDIRECTIONALLIGHT)
			elem2->SetAttribute("allowDirectionalLight", vItemDataHeaders[iCurItemData].allowDirectionalLight);
		if(vItemDataHeaders[iCurItemData].allowExplodeStreaks != DEFAULT_ALLOWEXPLODESTREAKS)
			elem2->SetAttribute("allowExplodeStreaks", vItemDataHeaders[iCurItemData].allowExplodeStreaks);
		if(vItemDataHeaders[iCurItemData].animThreshold != DEFAULT_ANIMTHRESHOLD)
			elem2->SetAttribute("animThreshold", vItemDataHeaders[iCurItemData].animThreshold);
		if(vItemDataHeaders[iCurItemData].burnSlowsAnim != DEFAULT_BURNSLOWSANIM)
			elem2->SetAttribute("burnSlowsAnim", vItemDataHeaders[iCurItemData].burnSlowsAnim);
		if(vItemDataHeaders[iCurItemData].canGetPlague != DEFAULT_CANGETPLAGUE)
			elem2->SetAttribute("canGetPlague", vItemDataHeaders[iCurItemData].canGetPlague);
		if(vItemDataHeaders[iCurItemData].collideEnvironment != DEFAULT_COLLIDEENVIRONMENT)
			elem2->SetAttribute("collideEnvironment", vItemDataHeaders[iCurItemData].collideEnvironment);
		if(vItemDataHeaders[iCurItemData].collideItems != DEFAULT_COLLIDEITEMS)
			elem2->SetAttribute("collideItems", vItemDataHeaders[iCurItemData].collideItems);
		elem2->SetAttribute("costCoins", vItemDataHeaders[iCurItemData].costCoins);
		if(vItemDataHeaders[iCurItemData].costStamps != DEFAULT_COSTSTAMPS)
			elem2->SetAttribute("costStamps", vItemDataHeaders[iCurItemData].costStamps);
		//elem2->SetAttribute("descid", vItemDataHeaders[iCurItemData].desc.id);
		//if(vItemDataHeaders[iCurItemData].desc.key != DEFAULT_DESCKEY)
		//	elem2->SetAttribute("desckey", vItemDataHeaders[iCurItemData].desc.key);
		if(vItemDataHeaders[iCurItemData].enableFreezePostAnim != DEFAULT_ENABLEFREEZEPOSTANIM)
			elem2->SetAttribute("enableFreezePostAnim", vItemDataHeaders[iCurItemData].enableFreezePostAnim);
		if(vItemDataHeaders[iCurItemData].enableHFlip != DEFAULT_ENABLEHFLIP)
			elem2->SetAttribute("enableHFlip", vItemDataHeaders[iCurItemData].enableHFlip);
		if(vItemDataHeaders[iCurItemData].floorWallShadow != DEFAULT_FLOORWALLSHADOW)
			elem2->SetAttribute("floorWallShadow", vItemDataHeaders[iCurItemData].floorWallShadow);
		if(vItemDataHeaders[iCurItemData].freezeOnCollide != DEFAULT_FREEZEONCOLLIDE)
			elem2->SetAttribute("freezeOnCollide", vItemDataHeaders[iCurItemData].freezeOnCollide);
		elem2->SetAttribute("iconAnimBoundsMax", vec2ToString(vItemDataHeaders[iCurItemData].iconAnimBoundsMax).c_str());
		elem2->SetAttribute("iconAnimBoundsMin", vec2ToString(vItemDataHeaders[iCurItemData].iconAnimBoundsMin).c_str());
		if(vItemDataHeaders[iCurItemData].illuminate != DEFAULT_ILLUMINATE)
			elem2->SetAttribute("illuminate", vItemDataHeaders[iCurItemData].illuminate);
		if(vItemDataHeaders[iCurItemData].initialBurnExportId != DEFAULT_INITIALBURNEXPORTID)
			elem2->SetAttribute("initialBurnExportId", vItemDataHeaders[iCurItemData].initialBurnExportId);
		if(vItemDataHeaders[iCurItemData].initialBurnPerGroup != DEFAULT_INITIALBURNPERGROUP)
			elem2->SetAttribute("initialBurnPerGroup", vItemDataHeaders[iCurItemData].initialBurnPerGroup);
		if(vItemDataHeaders[iCurItemData].instAshDoesSplat != DEFAULT_INSTASHDOESSPLAT)
			elem2->SetAttribute("instAshDoesSplat", vItemDataHeaders[iCurItemData].instAshDoesSplat);
		if(vItemDataHeaders[iCurItemData].instAshSoundResId != DEFAULT_INSTASHSOUNDRESID)
			elem2->SetAttribute("instAshSoundResId", vItemDataHeaders[iCurItemData].instAshSoundResId);
		if(vItemDataHeaders[iCurItemData].instantEffects != DEFAULT_INSTANTEFFECTS)
			elem2->SetAttribute("instantEffects", vItemDataHeaders[iCurItemData].instantEffects);
		//if(vItemDataHeaders[iCurItemData].itemIdStrId != DEFAULT_ITEMIDSTRID)
		//	elem2->SetAttribute("itemIdStrId", vItemDataHeaders[iCurItemData].itemIdStrId);
		if(vItemDataHeaders[iCurItemData].modXAmpMax != DEFAULT_MODXAMPMAX)
			elem2->SetAttribute("modXAmpMax", vItemDataHeaders[iCurItemData].modXAmpMax);
		if(vItemDataHeaders[iCurItemData].modXAmpMin != DEFAULT_MODXAMPMIN)
			elem2->SetAttribute("modXAmpMin", vItemDataHeaders[iCurItemData].modXAmpMin);
		if(vItemDataHeaders[iCurItemData].modXFreqMax != DEFAULT_MODXFREQMAX)
			elem2->SetAttribute("modXFreqMax", vItemDataHeaders[iCurItemData].modXFreqMax);
		if(vItemDataHeaders[iCurItemData].modXFreqMin != DEFAULT_MODXFREQMIN)
			elem2->SetAttribute("modXFreqMin", vItemDataHeaders[iCurItemData].modXFreqMin);
		if(vItemDataHeaders[iCurItemData].modXPhaseMax != DEFAULT_MODXPHASEMAX)
			elem2->SetAttribute("modXPhaseMax", vItemDataHeaders[iCurItemData].modXPhaseMax);
		if(vItemDataHeaders[iCurItemData].modXPhaseMin != DEFAULT_MODXPHASEMIN)
			elem2->SetAttribute("modXPhaseMin", vItemDataHeaders[iCurItemData].modXPhaseMin);
		if(vItemDataHeaders[iCurItemData].modXSpeedMax != DEFAULT_MODXSPEEDMAX)
			elem2->SetAttribute("modXSpeedMax", vItemDataHeaders[iCurItemData].modXSpeedMax);
		if(vItemDataHeaders[iCurItemData].modXSpeedMin != DEFAULT_MODXSPEEDMIN)
			elem2->SetAttribute("modXSpeedMin", vItemDataHeaders[iCurItemData].modXSpeedMin);
		if(vItemDataHeaders[iCurItemData].modYAmpMax != DEFAULT_MODYAMPMAX)
			elem2->SetAttribute("modYAmpMax", vItemDataHeaders[iCurItemData].modYAmpMax);
		if(vItemDataHeaders[iCurItemData].modYAmpMin != DEFAULT_MODYAMPMIN)
			elem2->SetAttribute("modYAmpMin", vItemDataHeaders[iCurItemData].modYAmpMin);
		if(vItemDataHeaders[iCurItemData].modYFreqMax != DEFAULT_MODYFREQMAX)
			elem2->SetAttribute("modYFreqMax", vItemDataHeaders[iCurItemData].modYFreqMax);
		if(vItemDataHeaders[iCurItemData].modYFreqMin != DEFAULT_MODYFREQMIN)
			elem2->SetAttribute("modYFreqMin", vItemDataHeaders[iCurItemData].modYFreqMin);
		if(vItemDataHeaders[iCurItemData].modYPhaseMax != DEFAULT_MODYPHASEMAX)
			elem2->SetAttribute("modYPhaseMax", vItemDataHeaders[iCurItemData].modYPhaseMax);
		if(vItemDataHeaders[iCurItemData].modYPhaseMin != DEFAULT_MODYPHASEMIN)
			elem2->SetAttribute("modYPhaseMin", vItemDataHeaders[iCurItemData].modYPhaseMin);
		if(vItemDataHeaders[iCurItemData].modYSpeedMax != DEFAULT_MODYSPEEDMAX)
			elem2->SetAttribute("modYSpeedMax", vItemDataHeaders[iCurItemData].modYSpeedMax);
		if(vItemDataHeaders[iCurItemData].modYSpeedMin != DEFAULT_MODYSPEEDMIN)
			elem2->SetAttribute("modYSpeedMin", vItemDataHeaders[iCurItemData].modYSpeedMin);
		if(vItemDataHeaders[iCurItemData].moneyItem != DEFAULT_MONEYITEM)
			elem2->SetAttribute("moneyItem", vItemDataHeaders[iCurItemData].moneyItem);
		if(vItemDataHeaders[iCurItemData].motorThreshold != DEFAULT_MOTORTHRESHOLD)
			elem2->SetAttribute("motorThreshold", vItemDataHeaders[iCurItemData].motorThreshold);
		if(vItemDataHeaders[iCurItemData].mouseGrabSoundResId != DEFAULT_MOUSEGRABSOUNDRESID)
			elem2->SetAttribute("mouseGrabSoundResId", vItemDataHeaders[iCurItemData].mouseGrabSoundResId);
		if(vItemDataHeaders[iCurItemData].mouseGrabbable != DEFAULT_MOUSEGRABBABLE)
			elem2->SetAttribute("mouseGrabbable", vItemDataHeaders[iCurItemData].mouseGrabbable);
		//elem2->SetAttribute("nameid", vItemDataHeaders[iCurItemData].name.id);
		//if(vItemDataHeaders[iCurItemData].name.key != DEFAULT_NAMEKEY)
		//	elem2->SetAttribute("namekey", vItemDataHeaders[iCurItemData].name.key);
		if(vItemDataHeaders[iCurItemData].orbitalGravity != DEFAULT_ORBITALGRAVITY)
			elem2->SetAttribute("orbitalGravity", vItemDataHeaders[iCurItemData].orbitalGravity);
		if(vItemDataHeaders[iCurItemData].plagueOnCollide != DEFAULT_PLAGUEONCOLLIDE)
			elem2->SetAttribute("plagueOnCollide", vItemDataHeaders[iCurItemData].plagueOnCollide);
		if(vItemDataHeaders[iCurItemData].popsCoins != DEFAULT_POPSCOINS)
			elem2->SetAttribute("popsCoins", vItemDataHeaders[iCurItemData].popsCoins);
		elem2->SetAttribute("purchaseCooldown", vItemDataHeaders[iCurItemData].purchaseCooldown);
		if(vItemDataHeaders[iCurItemData].quantity != DEFAULT_QUANTITY)
			elem2->SetAttribute("quantity", vItemDataHeaders[iCurItemData].quantity);
		if(vItemDataHeaders[iCurItemData].scaleVariance != DEFAULT_SCALEVARIANCE)
			elem2->SetAttribute("scaleVariance", vItemDataHeaders[iCurItemData].scaleVariance);
		elem2->SetAttribute("shipTimeSec", vItemDataHeaders[iCurItemData].shipTimeSec);
		if(vItemDataHeaders[iCurItemData].spawnLimitBurnExportId != DEFAULT_SPAWNLIMITBURNEXPORTID)
			elem2->SetAttribute("spawnLimitBurnExportId", vItemDataHeaders[iCurItemData].spawnLimitBurnExportId);
		if(vItemDataHeaders[iCurItemData].splitJumpLastFrame != DEFAULT_SPLITJUMPLASTFRAME)
			elem2->SetAttribute("splitJumpLastFrame", vItemDataHeaders[iCurItemData].splitJumpLastFrame);
		if(vItemDataHeaders[iCurItemData].uniqueIgniteSoundResId != DEFAULT_UNIQUEIGNITESOUNDRESID)
			elem2->SetAttribute("uniqueIgniteSoundResId", vItemDataHeaders[iCurItemData].uniqueIgniteSoundResId);
		if(vItemDataHeaders[iCurItemData].unlisted != DEFAULT_UNLISTED)
			elem2->SetAttribute("unlisted", vItemDataHeaders[iCurItemData].unlisted);
		elem2->SetAttribute("valueCoins", vItemDataHeaders[iCurItemData].valueCoins);
		if(vItemDataHeaders[iCurItemData].valueStamps != DEFAULT_VALUESTAMPS)
			elem2->SetAttribute("valueStamps", vItemDataHeaders[iCurItemData].valueStamps);
		
		//Create skeletons for this item
		XMLElement* elem3;
		for(list<skelsRecord>::iterator j = vSkeletonRecords[iCurItemData].begin(); j != vSkeletonRecords[iCurItemData].end(); j++)
		{
			elem3 = doc->NewElement("skeleton");
			elem3->SetAttribute("burnExport", j->burnExport);
			elem3->SetAttribute("selectWeight", j->selectWeight);
			elem3->SetAttribute("hasAnimThresh", j->hasAnimThresh);
			if(j->hasAnimThresh)
				elem3->SetAttribute("animThresh", j->animThresh);
			elem3->SetAttribute("animExportStrId", j->animExportStrId);
			elem3->SetAttribute("animBoundsMin", vec2ToString(j->animBoundsMin).c_str());
			elem3->SetAttribute("animBoundsMax", vec2ToString(j->animBoundsMax).c_str());
			
			//Add joints for this skeleton
			XMLElement* elem4;
			for(int k = j->firstJointIdx; k < j->firstJointIdx + j->numJoints; k++)
			{
				elem4 = doc->NewElement("joint");
				elem4->SetAttribute("boneIdx1", vJointRecords[iCurItemData][k].boneIdx[0]);
				elem4->SetAttribute("boneIdx2", vJointRecords[iCurItemData][k].boneIdx[1]);
				elem4->SetAttribute("boneBurnGridCellIdx1", vJointRecords[iCurItemData][k].boneBurnGridCellIdx[0]);
				elem4->SetAttribute("boneBurnGridCellIdx2", vJointRecords[iCurItemData][k].boneBurnGridCellIdx[1]);
				elem4->SetAttribute("burnable", vJointRecords[iCurItemData][k].burnable);
				elem4->SetAttribute("allowExtDamage", vJointRecords[iCurItemData][k].allowExtDamage);
				elem4->SetAttribute("modelSpacePos", vec2ToString(vJointRecords[iCurItemData][k].modelSpacePos).c_str());
				elem4->SetAttribute("strength", vJointRecords[iCurItemData][k].strength.value);
				elem4->SetAttribute("angleLimit", vJointRecords[iCurItemData][k].angleLimit.value);
				elem4->SetAttribute("speed", vJointRecords[iCurItemData][k].speed.value);
				elem4->SetAttribute("spin", vJointRecords[iCurItemData][k].spin.value);
				elem4->SetAttribute("wobble", vJointRecords[iCurItemData][k].wobble.value);
				elem3->InsertEndChild(elem4);
			}
			
			//Add bones for this skeleton
			for(int k = j->firstBoneIdx; k < j->firstBoneIdx + j->numBones; k++)
			{
				elem4 = doc->NewElement("bone");
				//Use defaults as we have them
				elem4->SetAttribute("angularDampEnumValId", vBoneRecords[iCurItemData][k].angularDampEnumValId);
				elem4->SetAttribute("animBlockIdx", vBoneRecords[iCurItemData][k].animBlockIdx);
				/*elem4->SetAttribute("animBlockTransform_11", vBoneRecords[iCurItemData][k].animBlockTransform._11);
				elem4->SetAttribute("animBlockTransform_12", vBoneRecords[iCurItemData][k].animBlockTransform._12);
				elem4->SetAttribute("animBlockTransform_13", vBoneRecords[iCurItemData][k].animBlockTransform._13);
				elem4->SetAttribute("animBlockTransform_21", vBoneRecords[iCurItemData][k].animBlockTransform._21);
				elem4->SetAttribute("animBlockTransform_22", vBoneRecords[iCurItemData][k].animBlockTransform._22);
				elem4->SetAttribute("animBlockTransform_23", vBoneRecords[iCurItemData][k].animBlockTransform._23);*/
				if(vBoneRecords[iCurItemData][k].applyGravityEnumValId != DEFAULT_APPLYGRAVITYENUMVALID)
					elem4->SetAttribute("applyGravityEnumValId", vBoneRecords[iCurItemData][k].applyGravityEnumValId);
				elem4->SetAttribute("ashBreakMaxAccelEnumValId", vBoneRecords[iCurItemData][k].ashBreakMaxAccelEnumValId);
				elem4->SetAttribute("ashBreakMinAccelEnumValId", vBoneRecords[iCurItemData][k].ashBreakMinAccelEnumValId);
				if(vBoneRecords[iCurItemData][k].ashSplitTimeBaseEnumValId != DEFAULT_ASHSPLITTIMEBASEENUMVALID)
					elem4->SetAttribute("ashSplitTimeBaseEnumValId", vBoneRecords[iCurItemData][k].ashSplitTimeBaseEnumValId);
				if(vBoneRecords[iCurItemData][k].ashSplitTimeVarEnumValId != DEFAULT_ASHSPLITTIMEVARENUMVALID)
					elem4->SetAttribute("ashSplitTimeVarEnumValId", vBoneRecords[iCurItemData][k].ashSplitTimeVarEnumValId);
				elem4->SetAttribute("attackAmountEnumValId", vBoneRecords[iCurItemData][k].attackAmountEnumValId);
				elem4->SetAttribute("attackSpeedEnumValId", vBoneRecords[iCurItemData][k].attackSpeedEnumValId);
				if(vBoneRecords[iCurItemData][k].autoRotateUprightEnumValId != DEFAULT_AUTOROTATEUPRIGHTENUMVALID)
					elem4->SetAttribute("autoRotateUprightEnumValId", vBoneRecords[iCurItemData][k].autoRotateUprightEnumValId);
				if(vBoneRecords[iCurItemData][k].behavior != DEFAULT_BEHAVIOR)
					elem4->SetAttribute("behavior", vBoneRecords[iCurItemData][k].behavior);
				elem4->SetAttribute("boneDensityEnumValId", vBoneRecords[iCurItemData][k].boneDensityEnumValId);
				elem4->SetAttribute("burnAmountEnumValId", vBoneRecords[iCurItemData][k].burnAmountEnumValId);
				elem4->SetAttribute("burnBoundsMax", vec2ToString(vBoneRecords[iCurItemData][k].burnBoundsMax).c_str());
				elem4->SetAttribute("burnBoundsMin", vec2ToString(vBoneRecords[iCurItemData][k].burnBoundsMin).c_str());
				//elem4->SetAttribute("burnGridHeight", vBoneRecords[iCurItemData][k].burnGridHeight);
				//elem4->SetAttribute("burnGridWidth", vBoneRecords[iCurItemData][k].burnGridWidth);
				elem4->SetAttribute("burnTimeEnumValId", vBoneRecords[iCurItemData][k].burnTimeEnumValId);
				elem4->SetAttribute("collideParticlesEnumValId", vBoneRecords[iCurItemData][k].collideParticlesEnumValId);
				elem4->SetAttribute("collideSoundEnumValId", vBoneRecords[iCurItemData][k].collideSoundEnumValId);
				if(vBoneRecords[iCurItemData][k].connectedGroupIdx != DEFAULT_CONNECTEDGROUPIDX)
					elem4->SetAttribute("connectedGroupIdx", vBoneRecords[iCurItemData][k].connectedGroupIdx);
				if(vBoneRecords[iCurItemData][k].decayParticlesEnumValId != DEFAULT_DECAYPARTICLESENUMVALID)
					elem4->SetAttribute("decayParticlesEnumValId", vBoneRecords[iCurItemData][k].decayParticlesEnumValId);
				elem4->SetAttribute("decaySpeedEnumValId", vBoneRecords[iCurItemData][k].decaySpeedEnumValId);
				if(vBoneRecords[iCurItemData][k].explodeIgnitePiecesEnumValId != DEFAULT_EXPLODEIGNITEPIECESENUMVALID)
					elem4->SetAttribute("explodeIgnitePiecesEnumValId", vBoneRecords[iCurItemData][k].explodeIgnitePiecesEnumValId);
				if(vBoneRecords[iCurItemData][k].explodeIgnoreBurnTriggerEnumValId != DEFAULT_EXPLODEIGNOREBURNTRIGGERENUMVALID)
					elem4->SetAttribute("explodeIgnoreBurnTriggerEnumValId", vBoneRecords[iCurItemData][k].explodeIgnoreBurnTriggerEnumValId);
				//elem4->SetAttribute("firstBurnUsedIdx", vBoneRecords[iCurItemData][k].firstBurnUsedIdx);
				//elem4->SetAttribute("firstPartTreeValIdx", vBoneRecords[iCurItemData][k].firstPartTreeValIdx);
				//elem4->SetAttribute("firstPartsIdx", vBoneRecords[iCurItemData][k].firstPartsIdx);
				//elem4->SetAttribute("firstRgnCellIdx", vBoneRecords[iCurItemData][k].firstRgnCellIdx);
				elem4->SetAttribute("frictionEnumValId", vBoneRecords[iCurItemData][k].frictionEnumValId);
				elem4->SetAttribute("id", vBoneRecords[iCurItemData][k].id);
				if(vBoneRecords[iCurItemData][k].igniteParticlesEnumValId != DEFAULT_IGNITEPARTICLESENUMVALID)
					elem4->SetAttribute("igniteParticlesEnumValId", vBoneRecords[iCurItemData][k].igniteParticlesEnumValId);
				elem4->SetAttribute("igniteSoundEnumValId", vBoneRecords[iCurItemData][k].igniteSoundEnumValId);
				elem4->SetAttribute("igniteTimeEnumValId", vBoneRecords[iCurItemData][k].igniteTimeEnumValId);
				if(vBoneRecords[iCurItemData][k].instAshOnCollideEnumValId != DEFAULT_INSTASHONCOLLIDEENUMVALID)
					elem4->SetAttribute("instAshOnCollideEnumValId", vBoneRecords[iCurItemData][k].instAshOnCollideEnumValId);
				elem4->SetAttribute("itemSpacePosition", vec2ToString(vBoneRecords[iCurItemData][k].itemSpacePosition).c_str());
				elem4->SetAttribute("linearDampEnumValId", vBoneRecords[iCurItemData][k].linearDampEnumValId);
				elem4->SetAttribute("mouseGrabSoundEnumValId", vBoneRecords[iCurItemData][k].mouseGrabSoundEnumValId);
				//elem4->SetAttribute("numPartTreeVals", vBoneRecords[iCurItemData][k].numPartTreeVals);
				//if(vBoneRecords[iCurItemData][k].numParts != DEFAULT_NUMPARTS)
				//	elem4->SetAttribute("numParts", vBoneRecords[iCurItemData][k].numParts);
				//if(vBoneRecords[iCurItemData][k].numRgnCells != DEFAULT_NUMRGNCELLS)
				//	elem4->SetAttribute("numRgnCells", vBoneRecords[iCurItemData][k].numRgnCells);
				if(vBoneRecords[iCurItemData][k].postExplodeAshBreakMinAccelEnumValId != DEFAULT_POSTEXPLODEASHBREAKMINACCELENUMVALID)
					elem4->SetAttribute("postExplodeAshBreakMinAccelEnumValId", vBoneRecords[iCurItemData][k].postExplodeAshBreakMinAccelEnumValId);
				if(vBoneRecords[iCurItemData][k].postExplodeSplitTimeBaseEnumValId != DEFAULT_POSTEXPLODESPLITTIMEBASEENUMVALID)
					elem4->SetAttribute("postExplodeSplitTimeBaseEnumValId", vBoneRecords[iCurItemData][k].postExplodeSplitTimeBaseEnumValId);
				if(vBoneRecords[iCurItemData][k].postExplodeSplitTimeVarEnumValId != DEFAULT_POSTEXPLODESPLITTIMEVARENUMVALID)
					elem4->SetAttribute("postExplodeSplitTimeVarEnumValId", vBoneRecords[iCurItemData][k].postExplodeSplitTimeVarEnumValId);
				elem4->SetAttribute("restitutionEnumValId", vBoneRecords[iCurItemData][k].restitutionEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpDoCamShakeEnumValId != DEFAULT_SHATTEREXPDOCAMSHAKEENUMVALID)
					elem4->SetAttribute("shatterExpDoCamShakeEnumValId", vBoneRecords[iCurItemData][k].shatterExpDoCamShakeEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpEffectEnumValId != DEFAULT_SHATTEREXPEFFECTENUMVALID)
					elem4->SetAttribute("shatterExpEffectEnumValId", vBoneRecords[iCurItemData][k].shatterExpEffectEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpFireAmountEnumValId != DEFAULT_SHATTEREXPFIREAMOUNTENUMVALID)
					elem4->SetAttribute("shatterExpFireAmountEnumValId", vBoneRecords[iCurItemData][k].shatterExpFireAmountEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpFireSpeedEnumValId != DEFAULT_SHATTEREXPFIRESPEEDENUMVALID)
					elem4->SetAttribute("shatterExpFireSpeedEnumValId", vBoneRecords[iCurItemData][k].shatterExpFireSpeedEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpForceEnumValId != DEFAULT_SHATTEREXPFORCEENUMVALID)
					elem4->SetAttribute("shatterExpForceEnumValId", vBoneRecords[iCurItemData][k].shatterExpForceEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpRadiusEnumValId != DEFAULT_SHATTEREXPRADIUSENUMVALID)
					elem4->SetAttribute("shatterExpRadiusEnumValId", vBoneRecords[iCurItemData][k].shatterExpRadiusEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpSoundEnumValId != DEFAULT_SHATTEREXPSOUNDENUMVALID)
					elem4->SetAttribute("shatterExpSoundEnumValId", vBoneRecords[iCurItemData][k].shatterExpSoundEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpTimeFactorEnumValId != DEFAULT_SHATTEREXPTIMEFACTORENUMVALID)
					elem4->SetAttribute("shatterExpTimeFactorEnumValId", vBoneRecords[iCurItemData][k].shatterExpTimeFactorEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpTimeHoldDownEnumValId != DEFAULT_SHATTEREXPTIMEHOLDDOWNENUMVALID)
					elem4->SetAttribute("shatterExpTimeHoldDownEnumValId", vBoneRecords[iCurItemData][k].shatterExpTimeHoldDownEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpTimeRampDownEnumValId != DEFAULT_SHATTEREXPTIMERAMPDOWNENUMVALID)
					elem4->SetAttribute("shatterExpTimeRampDownEnumValId", vBoneRecords[iCurItemData][k].shatterExpTimeRampDownEnumValId);
				if(vBoneRecords[iCurItemData][k].shatterExpTimeRampUpEnumValId != DEFAULT_SHATTEREXPTIMERAMPUPENUMVALID)
					elem4->SetAttribute("shatterExpTimeRampUpEnumValId", vBoneRecords[iCurItemData][k].shatterExpTimeRampUpEnumValId);
				if(vBoneRecords[iCurItemData][k].smearAmountEnumValId != DEFAULT_SMEARAMOUNTENUMVALID)
					elem4->SetAttribute("smearAmountEnumValId", vBoneRecords[iCurItemData][k].smearAmountEnumValId);
				if(vBoneRecords[iCurItemData][k].splatParticlesEnumValId != DEFAULT_SPLATPARTICLESENUMVALID)
					elem4->SetAttribute("splatParticlesEnumValId", vBoneRecords[iCurItemData][k].splatParticlesEnumValId);
				if(vBoneRecords[iCurItemData][k].splitBrittleEnumValId != DEFAULT_SPLITBRITTLEENUMVALID)
					elem4->SetAttribute("splitBrittleEnumValId", vBoneRecords[iCurItemData][k].splitBrittleEnumValId);
				if(vBoneRecords[iCurItemData][k].splitDespawnEffectEnumValId != DEFAULT_SPLITDESPAWNEFFECTENUMVALID)
					elem4->SetAttribute("splitDespawnEffectEnumValId", vBoneRecords[iCurItemData][k].splitDespawnEffectEnumValId);
				if(vBoneRecords[iCurItemData][k].splitEffectEnumValId != DEFAULT_SPLITEFFECTENUMVALID)
					elem4->SetAttribute("splitEffectEnumValId", vBoneRecords[iCurItemData][k].splitEffectEnumValId);
				if(vBoneRecords[iCurItemData][k].splitSFXLargeEnumValId != DEFAULT_SPLITSFXLARGEENUMVALID)
					elem4->SetAttribute("splitSFXLargeEnumValId", vBoneRecords[iCurItemData][k].splitSFXLargeEnumValId);
				if(vBoneRecords[iCurItemData][k].splitSFXMediumEnumValId != DEFAULT_SPLITSFXMEDIUMENUMVALID)
					elem4->SetAttribute("splitSFXMediumEnumValId", vBoneRecords[iCurItemData][k].splitSFXMediumEnumValId);
				if(vBoneRecords[iCurItemData][k].splitSFXSmallEnumValId != DEFAULT_SPLITSFXSMALLENUMVALID)
					elem4->SetAttribute("splitSFXSmallEnumValId", vBoneRecords[iCurItemData][k].splitSFXSmallEnumValId);
				if(vBoneRecords[iCurItemData][k].splitThresholdEnumValId != DEFAULT_SPLITTHRESHOLDENUMVALID)
					elem4->SetAttribute("splitThresholdEnumValId", vBoneRecords[iCurItemData][k].splitThresholdEnumValId);
				elem4->SetAttribute("stampBlackWhitePctEnumValId", vBoneRecords[iCurItemData][k].stampBlackWhitePctEnumValId);
				
				//Add shapes for this bone
				XMLElement* elem5;
				for(int l = vBoneRecords[iCurItemData][k].firstBoneMainShapeIdx; l < vBoneRecords[iCurItemData][k].firstBoneMainShapeIdx + vBoneRecords[iCurItemData][k].numBoneMainShapes; l++)
				{
					elem5 = doc->NewElement("shape");
					
					if(vBoneShapes[iCurItemData][l].flags == TYPE_POLYGON)	//Polygon shape
					{
						elem5->SetAttribute("type", "polygon");
						//Write vertices for this shape
						XMLElement* elem6;
						for(int m = 0; m < vBoneShapes[iCurItemData][m].numVerts; m++)
						{
							elem6 = doc->NewElement("vert");
							
							elem6->SetAttribute("pos", vec2ToString(vBoneShapes[iCurItemData][m].verts[m]).c_str());
							
							elem5->InsertEndChild(elem6);
						}
					}
					else if(vBoneShapes[iCurItemData][l].flags == TYPE_CIRCLE)	//Circle shape
					{
						elem5->SetAttribute("type", "circle");
						//Write vertices for this shape
						XMLElement* elem6;
						elem6 = doc->NewElement("center");
						elem6->SetAttribute("pos", vec2ToString(vBoneShapes[iCurItemData][l].verts[0]).c_str());
						elem5->InsertEndChild(elem6);
						elem6 = doc->NewElement("radius");
						elem6->SetAttribute("value", vBoneShapes[iCurItemData][l].verts[1].x);
						elem5->InsertEndChild(elem6);
					}
					
					elem4->InsertEndChild(elem5);
				}
				
				elem3->InsertEndChild(elem4);
				
				//Add bone parts
				if(vBoneRecords[iCurItemData][k].numParts)	//if there are any
				{
					elem5 = doc->NewElement("parts");
					for(int l = vBoneRecords[iCurItemData][k].firstPartsIdx; l < vBoneRecords[iCurItemData][k].firstPartsIdx + vBoneRecords[iCurItemData][k].numParts; l++)
					{
						XMLElement* elem6 = doc->NewElement("part");
						elem6->SetAttribute("flags", vBoneParts[iCurItemData][l].flags);
						elem6->SetAttribute("texId", vBoneParts[iCurItemData][l].texResId);
						elem6->SetAttribute("normId", vBoneParts[iCurItemData][l].normalMapResId);
						elem6->SetAttribute("pupilRange", vBoneParts[iCurItemData][l].pupilMoveRange);
						elem5->InsertEndChild(elem6);
					}
					elem4->InsertEndChild(elem5);
				}
				
				//Add bone part tree
				if(vBoneRecords[iCurItemData][k].numPartTreeVals)	//if there are any
				{
					elem5 = doc->NewElement("parttree");	//AND A PARTRIDGE IN A PART TREE
					for(int l = vBoneRecords[iCurItemData][k].firstPartTreeValIdx; l < vBoneRecords[iCurItemData][k].firstPartTreeValIdx + vBoneRecords[iCurItemData][k].numPartTreeVals; l++)
					{
						XMLElement* elem6 = doc->NewElement("value");
						elem6->SetAttribute("val", vBonePartTreeValues[iCurItemData][l]);
						elem5->InsertEndChild(elem6);
					}
					elem4->InsertEndChild(elem5);
				}
				//TODO when reading back in, else firstPartTreeValIdx is -1
				
				//Add mapping regions
				if(vBoneRecords[iCurItemData][k].numRgnCells)
				{
					elem5 = doc->NewElement("regions");
					for(int l = vBoneRecords[iCurItemData][k].firstRgnCellIdx; l < vBoneRecords[iCurItemData][k].firstRgnCellIdx + vBoneRecords[iCurItemData][k].numRgnCells; l++)
					{
						XMLElement* elem6 = doc->NewElement("regionmapping");
						elem6->SetAttribute("uid", vGridCellMappings[iCurItemData][l].regionUID);
						elem6->SetAttribute("index", vGridCellMappings[iCurItemData][l].burnGridCellIdx);						
						elem5->InsertEndChild(elem6);
					}
					elem4->InsertEndChild(elem5);
				}
				
				//Add burn grid
				if(vBoneRecords[iCurItemData][k].burnGridWidth && vBoneRecords[iCurItemData][k].burnGridHeight)
				{
					elem5 = doc->NewElement("burngrid");
					elem5->SetAttribute("firstpos", vBoneRecords[iCurItemData][k].firstBurnUsedIdx);
					if(vBoneRecords[iCurItemData][k].burnGridSize != DEFAULT_BURNGRIDSIZE)
						elem5->SetAttribute("size", vBoneRecords[iCurItemData][k].burnGridSize);
					elem5->SetAttribute("width", vBoneRecords[iCurItemData][k].burnGridWidth);
					elem5->SetAttribute("height", vBoneRecords[iCurItemData][k].burnGridHeight);
					wstring s;
					for(int l = 0; l < vBoneRecords[iCurItemData][k].burnGridWidth * vBoneRecords[iCurItemData][k].burnGridHeight; l++)
					{
						if(vBurnGrid[iCurItemData][l])
							s.push_back(TEXT('1'));
						else
							s.push_back(TEXT('0'));
					}
					elem5->SetAttribute("grid", ws2s(s).c_str());
					elem4->InsertEndChild(elem5);
				}
				
				//DEBUG Now loop back through here and pull data from it all
				//for(const XMLAttribute* att = elem4->FirstAttribute(); att != NULL; att = att->Next())
				//	mOccurrences[att->Name()].push_back(att->Value());
				
				//Write matrix stuff
				WriteTransforms(elem4, doc, vBoneRecords[iCurItemData][k].animBlockTransform);
			}
			
			elem2->InsertEndChild(elem3);
		}
		
		//Add string table
		for(int l = 0; l < vStringTableEntries[iCurItemData].size(); l++)
		{
			if(l == vItemDataHeaders[iCurItemData].itemIdStrId)
				elem3 = doc->NewElement("id");
			else if(l == vItemDataHeaders[iCurItemData].name.key)
			{
				elem3 = doc->NewElement("name");
				elem3->SetAttribute("strid", vItemDataHeaders[iCurItemData].name.id);
			}
			else if(l == vItemDataHeaders[iCurItemData].desc.key)
			{
				elem3 = doc->NewElement("description");
				elem3->SetAttribute("strid", vItemDataHeaders[iCurItemData].desc.id);
			}
			else
				elem3 = doc->NewElement("text");
			for(int m = vStringTableEntries[iCurItemData][l].pointerIndex; m < vStringTableEntries[iCurItemData][l].pointerIndex + vStringTableEntries[iCurItemData][l].pointerCount; m++)
			{
				XMLElement* elem6 = doc->NewElement("string");
				elem6->SetAttribute("lang", ws2s(toLangString(vStringPointerEntries[iCurItemData][m].languageId)).c_str());
				elem6->SetAttribute("data", &(vStrings[iCurItemData].data()[vStringPointerEntries[iCurItemData][m].offset]));
				elem3->InsertEndChild(elem6);
			}
			elem2->InsertEndChild(elem3);
		}
		
		//DEBUG: Write item ID and name
		//onames << "{967411u,\"data/items/SpiderEggSpider/colorbgicon\"},"
		//onames << "{" << i->itemId << "u,\"" << ws2s(mItemNames[i->itemId]) << "\"}," << endl;// 967411u,\"data/items/SpiderEggSpider/colorbgicon\"},"
		
		iCurItemData++;
		//TODO: Write rest of XML stuff for rest of item data
		elem->InsertEndChild(elem2);
		#ifndef SPLIT_XML_FILES
		root->InsertEndChild(elem);
		#else
		doc->InsertFirstChild(elem);
		makeFolder(sXMLFilename);	//Create folders if they're not there, or next line will silently fail
		doc->SaveFile(ws2s(sXMLFilename).c_str());
		delete doc;
		#endif
	}
	
	//DEBUG
	//onames << "};" << endl << endl << endl << "#endif" << endl;
	//onames.close();
	//oHash.close();
	#ifndef SPLIT_XML_FILES
	doc->InsertFirstChild(root);
	doc->SaveFile(ws2s(sFilename).c_str());
	delete doc;
	#else
	itemmanifest->InsertEndChild(itemroot);
	itemmanifest->SaveFile(ws2s(sFilename).c_str());
	delete itemmanifest;
	#endif
	
	//DEBUG: Save out all the data that we got in our map
	/*ofstream ofile("map2.txt");
	ofstream ofMap("map.txt");
	ofstream ofCode("mapcode.txt");
	for(map<string, list<string> >::iterator i = mOccurrences.begin(); i != mOccurrences.end(); i++)
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
	//
	
	return true;
}

//TODO Fix problems and stuff for next release
bool XMLToItemManifest(const wchar_t* cFilename)
{
	//Open this XML file for parsing
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
	
	//Get list of filenames to read in
	list<wstring> lItemManifestFilenames;
	for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		const char* filename = elem->Attribute("path");
		if(filename != NULL)
			lItemManifestFilenames.push_back(s2ws(filename));
	}
	
	delete doc;	//Done with this file
	
	//Now loop through our files, reading in the data from each
	list<itemManifestRecord> lItemManifests;
	list<normalDependency> lNormalDeps;
	list<soundDependency> lSoundDeps;
	list<effectDependency> lEffectDeps;
	list<itemDependency> lItemDeps;
	u32 binDataRunningTally = 0;	//Offset into the binary data for each item
	vector<itemDataHeader> lItemData;	//Everything here and below should be vectors so I can access as needed
	vector<list<skelsRecord> > vSkeletons;
	vector<list<jointRecord> > vJoints;
	vector<list<boneRecord> > vBones;
	vector<list<boneShapeRecord> > vBoneShapes;
	vector<list<bonePartRecord> > vBoneParts;
	vector<list<i32> > vBonePartTreeValues;
	vector<list<byte> > vBurnGrid;
	vector<StringTableHeader> vStringTableHeaders;
	vector<list<StringTableEntry> > vlStringTables;
	vector<list<StringPointerEntry> > vlStringPointers;
	vector<list<char> > vlStrings;
	vector<list<boneGridCellMappingRegion> > vBoneRgnCells;
	
	for(list<wstring>::iterator i = lItemManifestFilenames.begin(); i != lItemManifestFilenames.end(); i++)
	{
		doc = new XMLDocument;
		int iErr = doc->LoadFile(ws2s(*i).c_str());
		if(iErr != XML_NO_ERROR)
		{
			cout << "Error parsing XML file " << ws2s(*i) << ": Error " << iErr << endl;
			delete doc;
			return false;
		}
		
		root = doc->RootElement();
		
		//Read in itemManifestRecord
		itemManifestRecord imr;
		if(root->QueryUnsignedAttribute("id", &imr.itemId) != XML_NO_ERROR)
		{
			cout << "Unable to read item ID from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		
		//animresid
		XMLElement* elem = root->FirstChildElement("animresid");
		if(elem == NULL)
		{
			cout << "Unable to read item animresid from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		const char* cFilename = elem->Attribute("filename");
		if(cFilename == NULL)
		{
			cout << "Unable to read item animresid filename from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		imr.animResId = getResID(s2ws(cFilename));
		
		//recentlymodifiedrank
		imr.recentlyModifiedRank = 0;	//For debug purposes, anyway; we don't care if it isn't here
		elem = root->FirstChildElement("recentlymodifiedrank");
		if(elem != NULL)
			elem->QueryIntAttribute("value", &imr.recentlyModifiedRank);
		
		//coloritemicon
		elem = root->FirstChildElement("coloritemicon");
		if(elem == NULL)
		{
			cout << "Unable to read item coloritemicon from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		cFilename = elem->Attribute("filename");
		if(cFilename == NULL)
		{
			cout << "Unable to read item coloritemicon filename from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		imr.catalogIconColorItemTexResId = getResID(s2ws(cFilename));
		
		//colorbgicon
		elem = root->FirstChildElement("colorbgicon");
		if(elem == NULL)
		{
			cout << "Unable to read item colorbgicon from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		cFilename = elem->Attribute("filename");
		if(cFilename == NULL)
		{
			cout << "Unable to read item colorbgicon filename from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		imr.catalogIconColorBGTexResId = getResID(s2ws(cFilename));
		
		//greybgicon
		elem = root->FirstChildElement("greybgicon");
		if(elem == NULL)
		{
			cout << "Unable to read item greybgicon from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		cFilename = elem->Attribute("filename");
		if(cFilename == NULL)
		{
			cout << "Unable to read item greybgicon filename from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		imr.catalogIconGreyBGTexResId = getResID(s2ws(cFilename));
		
		//Dependencies
		imr.firstNormalDepends = lNormalDeps.size();
		imr.firstSoundDepends = lSoundDeps.size();
		imr.firstEffectDepends = lEffectDeps.size();
		imr.firstItemDepends = lItemDeps.size();
		imr.numNormalDepends = imr.numSoundDepends = imr.numEffectDepends = imr.numItemDepends = 0;
		elem = root->FirstChildElement("depends");
		if(elem != NULL)
		{
			//Add normal dependencies
			for(XMLElement* elem2 = elem->FirstChildElement("normal"); elem2 != NULL; elem2 = elem2->NextSiblingElement("normal"))
			{
				normalDependency nd;
				imr.numNormalDepends++;
				const char* cIDFilename = elem2->Attribute("filename");
				if(cIDFilename == NULL)
				{
					cout << "Unable to read normal dependency filename from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				nd.normalTexResId = getResID(s2ws(cIDFilename));
				lNormalDeps.push_back(nd);
			}
			
			//Add sound dependencies
			for(XMLElement* elem2 = elem->FirstChildElement("sound"); elem2 != NULL; elem2 = elem2->NextSiblingElement("sound"))
			{
				soundDependency sd;
				imr.numSoundDepends++;
				const char* cIDFilename = elem2->Attribute("filename");
				if(cIDFilename == NULL)
				{
					cout << "Unable to read sound dependency filename from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				sd.soundResId = getResID(s2ws(cIDFilename));
				lSoundDeps.push_back(sd);
			}
			
			//Add effect dependencies
			for(XMLElement* elem2 = elem->FirstChildElement("effect"); elem2 != NULL; elem2 = elem2->NextSiblingElement("effect"))
			{
				effectDependency ed;
				imr.numEffectDepends++;
				const char* cIDFilename = elem2->Attribute("filename");
				if(cIDFilename == NULL)
				{
					cout << "Unable to read effect dependency filename from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				ed.effectResId = getResID(s2ws(cIDFilename));
				lEffectDeps.push_back(ed);
			}
			
			//Add item dependencies
			for(XMLElement* elem2 = elem->FirstChildElement("item"); elem2 != NULL; elem2 = elem2->NextSiblingElement("item"))
			{
				itemDependency itd;
				imr.numItemDepends++;
				const char* cID = elem2->Attribute("id");
				if(cID == NULL)
				{
					cout << "Unable to read item dependency id from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				itd.itemResId = itemNameToID(cID);
				lItemDeps.push_back(itd);
			}
		}
		
		//Binary data offset
		imr.binDataOffsetBytes = binDataRunningTally;
		
		//Binary data header
		itemDataHeader idh;
		
		elem = root->FirstChildElement("itemdata");
		if(elem == NULL)
		{
			cout << "Unable to read itemdata from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		
		//fields with fairly common defaults
		idh.absPosition = DEFAULT_ABSPOSITION;
		idh.allowDirectionalLight = DEFAULT_ALLOWDIRECTIONALLIGHT;
		idh.allowExplodeStreaks = DEFAULT_ALLOWEXPLODESTREAKS;
		idh.animThreshold = DEFAULT_ANIMTHRESHOLD;
		idh.burnSlowsAnim = DEFAULT_BURNSLOWSANIM;
		idh.canGetPlague = DEFAULT_CANGETPLAGUE;
		idh.collideEnvironment = DEFAULT_COLLIDEENVIRONMENT;
		idh.collideItems = DEFAULT_COLLIDEITEMS;
		idh.costStamps = DEFAULT_COSTSTAMPS;
		idh.enableFreezePostAnim = DEFAULT_ENABLEFREEZEPOSTANIM;
		idh.enableHFlip = DEFAULT_ENABLEHFLIP;
		idh.floorWallShadow = DEFAULT_FLOORWALLSHADOW;
		idh.freezeOnCollide = DEFAULT_FREEZEONCOLLIDE;
		idh.illuminate = DEFAULT_ILLUMINATE;
		idh.initialBurnExportId = DEFAULT_INITIALBURNEXPORTID;
		idh.initialBurnPerGroup = DEFAULT_INITIALBURNPERGROUP;
		idh.instAshDoesSplat = DEFAULT_INSTASHDOESSPLAT;
		idh.instAshSoundResId = DEFAULT_INSTASHSOUNDRESID;
		idh.instantEffects = DEFAULT_INSTANTEFFECTS;
		idh.modXAmpMax = DEFAULT_MODXAMPMAX;
		idh.modXAmpMin = DEFAULT_MODXAMPMIN;
		idh.modXFreqMax = DEFAULT_MODXFREQMAX;
		idh.modXFreqMin = DEFAULT_MODXFREQMIN;
		idh.modXPhaseMax = DEFAULT_MODXPHASEMAX;
		idh.modXPhaseMin = DEFAULT_MODXPHASEMIN;
		idh.modXSpeedMax = DEFAULT_MODXSPEEDMAX;
		idh.modXSpeedMin = DEFAULT_MODXSPEEDMIN;
		idh.modYAmpMax = DEFAULT_MODYAMPMAX;
		idh.modYAmpMin = DEFAULT_MODYAMPMIN;
		idh.modYFreqMax = DEFAULT_MODYFREQMAX;
		idh.modYFreqMin = DEFAULT_MODYFREQMIN;
		idh.modYPhaseMax = DEFAULT_MODYPHASEMAX;
		idh.modYPhaseMin = DEFAULT_MODYPHASEMIN;
		idh.modYSpeedMax = DEFAULT_MODYSPEEDMAX;
		idh.modYSpeedMin = DEFAULT_MODYSPEEDMIN;
		idh.moneyItem = DEFAULT_MONEYITEM;
		idh.motorThreshold = DEFAULT_MOTORTHRESHOLD;
		idh.mouseGrabSoundResId = DEFAULT_MOUSEGRABSOUNDRESID;
		idh.mouseGrabbable = DEFAULT_MOUSEGRABBABLE;
		idh.orbitalGravity = DEFAULT_ORBITALGRAVITY;
		idh.plagueOnCollide = DEFAULT_PLAGUEONCOLLIDE;
		idh.popsCoins = DEFAULT_POPSCOINS;
		idh.quantity = DEFAULT_QUANTITY;
		idh.scaleVariance = DEFAULT_SCALEVARIANCE;
		idh.spawnLimitBurnExportId = DEFAULT_SPAWNLIMITBURNEXPORTID;
		idh.splitJumpLastFrame = DEFAULT_SPLITJUMPLASTFRAME;
		idh.uniqueIgniteSoundResId = DEFAULT_UNIQUEIGNITESOUNDRESID;
		idh.unlisted = DEFAULT_UNLISTED;
		idh.valueStamps = DEFAULT_VALUESTAMPS;
		
		//Redundant fields
		idh.itemId = imr.itemId;
		idh.animResId = imr.animResId;
		
		//Will fail if these fields don't exist in the XML
		idh.costCoins = 0;
		idh.purchaseCooldown = 0;
		idh.shipTimeSec = 0;
		idh.valueCoins = 0;
		idh.iconAnimBoundsMax.x = 0;
		idh.iconAnimBoundsMax.y = 0;
		idh.iconAnimBoundsMin.x = 0;
		idh.iconAnimBoundsMin.y = 0;
		
		//Read mandatory fields; error out on fail
		if(elem->QueryIntAttribute("costCoins", &idh.costCoins) != XML_NO_ERROR)
		{
			cout << "Unable to read coin cost from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		if(elem->QueryFloatAttribute("purchaseCooldown", &idh.purchaseCooldown) != XML_NO_ERROR)
		{
			cout << "Unable to read purchase cooldown from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		if(elem->QueryIntAttribute("shipTimeSec", &idh.shipTimeSec) != XML_NO_ERROR)
		{
			cout << "Unable to read shipping time from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		if(elem->QueryIntAttribute("valueCoins", &idh.valueCoins) != XML_NO_ERROR)
		{
			cout << "Unable to read coin value from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		const char* cVec = elem->Attribute("iconAnimBoundsMin");
		if(cVec == NULL)
		{
			cout << "Unable to read iconAnimBoundsMin from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		idh.iconAnimBoundsMin = stringToVec2(cVec);
		cVec = elem->Attribute("iconAnimBoundsMax");
		if(cVec == NULL)
		{
			cout << "Unable to read iconAnimBoundsMax from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		idh.iconAnimBoundsMax = stringToVec2(cVec);
		
		//Read optional fields; ignore errors
		elem->QueryIntAttribute("costStamps", &idh.costStamps);
		elem->QueryIntAttribute("valueStamps", &idh.valueStamps);
		elem->QueryIntAttribute("unlisted", &idh.unlisted);
		elem->QueryIntAttribute("popsCoins", &idh.popsCoins);
		elem->QueryIntAttribute("moneyItem", &idh.moneyItem);
		elem->QueryIntAttribute("absPosition", &idh.absPosition);
		elem->QueryIntAttribute("quantity", &idh.quantity);
		elem->QueryIntAttribute("initialBurnPerGroup", &idh.initialBurnPerGroup);
		elem->QueryIntAttribute("mouseGrabbable", &idh.mouseGrabbable);
		elem->QueryIntAttribute("enableHFlip", &idh.enableHFlip);
		elem->QueryIntAttribute("floorWallShadow", &idh.floorWallShadow);
		elem->QueryIntAttribute("splitJumpLastFrame", &idh.splitJumpLastFrame);
		elem->QueryIntAttribute("allowDirectionalLight", &idh.allowDirectionalLight);
		elem->QueryIntAttribute("instantEffects", &idh.instantEffects);
		elem->QueryIntAttribute("freezeOnCollide", &idh.freezeOnCollide);
		elem->QueryIntAttribute("enableFreezePostAnim", &idh.enableFreezePostAnim);
		elem->QueryIntAttribute("collideItems", &idh.collideItems);
		elem->QueryIntAttribute("collideEnvironment", &idh.collideEnvironment);
		elem->QueryIntAttribute("orbitalGravity", &idh.orbitalGravity);
		elem->QueryIntAttribute("allowExplodeStreaks", &idh.allowExplodeStreaks);
		elem->QueryIntAttribute("burnSlowsAnim", &idh.burnSlowsAnim);
		elem->QueryIntAttribute("plagueOnCollide", &idh.plagueOnCollide);
		elem->QueryIntAttribute("canGetPlague", &idh.canGetPlague);
		elem->QueryIntAttribute("instAshDoesSplat", &idh.instAshDoesSplat);
		elem->QueryUnsignedAttribute("initialBurnExportId", &idh.initialBurnExportId);
		elem->QueryUnsignedAttribute("mouseGrabSoundResId", &idh.mouseGrabSoundResId);
		elem->QueryUnsignedAttribute("uniqueIgniteSoundResId", &idh.uniqueIgniteSoundResId);
		elem->QueryUnsignedAttribute("spawnLimitBurnExportId", &idh.spawnLimitBurnExportId);
		elem->QueryUnsignedAttribute("instAshSoundResId", &idh.instAshSoundResId);
		elem->QueryFloatAttribute("animThreshold", &idh.animThreshold);
		elem->QueryFloatAttribute("motorThreshold", &idh.motorThreshold);
		elem->QueryFloatAttribute("scaleVariance", &idh.scaleVariance);
		elem->QueryFloatAttribute("illuminate", &idh.illuminate);
		elem->QueryFloatAttribute("modXAmpMin", &idh.modXAmpMin);
		elem->QueryFloatAttribute("modXAmpMax", &idh.modXAmpMax);
		elem->QueryFloatAttribute("modXFreqMin", &idh.modXFreqMin);
		elem->QueryFloatAttribute("modXFreqMax", &idh.modXFreqMax);
		elem->QueryFloatAttribute("modXPhaseMin", &idh.modXPhaseMin);
		elem->QueryFloatAttribute("modXPhaseMax", &idh.modXPhaseMax);
		elem->QueryFloatAttribute("modXSpeedMin", &idh.modXSpeedMin);
		elem->QueryFloatAttribute("modXSpeedMax", &idh.modXSpeedMax);
		elem->QueryFloatAttribute("modYAmpMin", &idh.modYAmpMin);
		elem->QueryFloatAttribute("modYAmpMax", &idh.modYAmpMax);
		elem->QueryFloatAttribute("modYFreqMin", &idh.modYFreqMin);
		elem->QueryFloatAttribute("modYFreqMax", &idh.modYFreqMax);
		elem->QueryFloatAttribute("modYPhaseMin", &idh.modYPhaseMin);
		elem->QueryFloatAttribute("modYPhaseMax", &idh.modYPhaseMax);
		elem->QueryFloatAttribute("modYSpeedMin", &idh.modYSpeedMin);
		elem->QueryFloatAttribute("modYSpeedMax", &idh.modYSpeedMax);
		
		//Read in skeletons for this itemdata
		list<skelsRecord> lSkels;
		list<jointRecord> lJoints;
		list<boneRecord> lBones;
		list<boneShapeRecord> lBoneShapes;
		list<bonePartRecord> lBoneParts;
		list<i32> lBonePartTreeValues;
		list<byte> lBurnGrid;
		list<boneGridCellMappingRegion> lBoneRgnCells;
		for(XMLElement* skeletons = elem->FirstChildElement("skeleton"); skeletons != NULL; skeletons = skeletons->NextSiblingElement("skeleton"))
		{
			skelsRecord sr;
			sr.firstJointIdx = lJoints.size();
			sr.numJoints = 0;
			sr.firstBoneIdx = lBones.size();
			sr.numBones = 0;
			
			if(skeletons->QueryUnsignedAttribute("burnExport", &sr.burnExport) != XML_NO_ERROR)
			{
				cout << "Unable to read skeleton burn export from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			if(skeletons->QueryFloatAttribute("selectWeight", &sr.selectWeight) != XML_NO_ERROR)
			{
				cout << "Unable to read skeleton selectWeight from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			if(skeletons->QueryIntAttribute("hasAnimThresh", &sr.hasAnimThresh) != XML_NO_ERROR)
			{
				cout << "Unable to read skeleton hasAnimThresh from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			//If has anim threshold set, query that as well
			if(sr.hasAnimThresh)
			{
				if(skeletons->QueryFloatAttribute("animThresh", &sr.animThresh) != XML_NO_ERROR)
				{
					cout << "Skeleton's hasAnimThresh set to nonzero, but unable to read animThresh from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
			}
			else
				sr.animThresh = 0.0f;
			if(skeletons->QueryIntAttribute("animExportStrId", &sr.animExportStrId) != XML_NO_ERROR)
			{
				cout << "Unable to read skeleton animExportStrId from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			
			cVec = skeletons->Attribute("animBoundsMin");
			if(cVec == NULL)
			{
				cout << "Unable to read animBoundsMin from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			sr.animBoundsMin = stringToVec2(cVec);
			cVec = skeletons->Attribute("animBoundsMax");
			if(cVec == NULL)
			{
				cout << "Unable to read animBoundsMax from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			sr.animBoundsMax = stringToVec2(cVec);
			
			for(XMLElement* joint = skeletons->FirstChildElement("joint"); joint != NULL; joint = joint->NextSiblingElement("joint"))
			{
				jointRecord jr;
				jr.strength.tuneId = jr.angleLimit.tuneId = jr.speed.tuneId = jr.spin.tuneId = jr.wobble.tuneId = 0;	//Don't care about these
				
				if(joint->QueryIntAttribute("boneIdx1", &jr.boneIdx[0]) != XML_NO_ERROR)
				{
					cout << "Unable to read joint bone index 1 from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryIntAttribute("boneIdx2", &jr.boneIdx[1]) != XML_NO_ERROR)
				{
					cout << "Unable to read joint bone index 2 from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryIntAttribute("boneBurnGridCellIdx1", &jr.boneBurnGridCellIdx[0]) != XML_NO_ERROR)
				{
					cout << "Unable to read joint bone burn grid cell index 1 from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryIntAttribute("boneBurnGridCellIdx2", &jr.boneBurnGridCellIdx[1]) != XML_NO_ERROR)
				{
					cout << "Unable to read joint bone burn grid cell index 2 from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryIntAttribute("burnable", &jr.burnable) != XML_NO_ERROR)
				{
					cout << "Unable to read joint burnable from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryIntAttribute("allowExtDamage", &jr.allowExtDamage) != XML_NO_ERROR)
				{
					cout << "Unable to read joint allowExtDamage from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				cVec = joint->Attribute("modelSpacePos");
				if(cVec == NULL)
				{
					cout << "Unable to read modelSpacePos from joint in XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				jr.modelSpacePos = stringToVec2(cVec);
				if(joint->QueryFloatAttribute("strength", &jr.strength.value) != XML_NO_ERROR)
				{
					cout << "Unable to read joint strength from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryFloatAttribute("angleLimit", &jr.angleLimit.value) != XML_NO_ERROR)
				{
					cout << "Unable to read joint angleLimit from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryFloatAttribute("speed", &jr.speed.value) != XML_NO_ERROR)
				{
					cout << "Unable to read joint speed from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryFloatAttribute("spin", &jr.spin.value) != XML_NO_ERROR)
				{
					cout << "Unable to read joint spin from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(joint->QueryFloatAttribute("wobble", &jr.wobble.value) != XML_NO_ERROR)
				{
					cout << "Unable to read joint wobble from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}				
				
				sr.numJoints++;
				lJoints.push_back(jr);
			}
			
			//TODO read children bones
			for(XMLElement* bone = skeletons->FirstChildElement("bone"); bone != NULL; bone = bone->NextSiblingElement("bone"))
			{
				boneRecord br;
				
				//Set to defaults, and ignore errors for these
				br.applyGravityEnumValId = DEFAULT_APPLYGRAVITYENUMVALID;
				bone->QueryUnsignedAttribute("applyGravityEnumValId", &br.applyGravityEnumValId);
				br.ashSplitTimeBaseEnumValId = DEFAULT_ASHSPLITTIMEBASEENUMVALID;
				bone->QueryUnsignedAttribute("ashSplitTimeBaseEnumValId", &br.ashSplitTimeBaseEnumValId);
				br.ashSplitTimeVarEnumValId = DEFAULT_ASHSPLITTIMEVARENUMVALID;
				bone->QueryUnsignedAttribute("ashSplitTimeVarEnumValId", &br.ashSplitTimeVarEnumValId);
				br.autoRotateUprightEnumValId = DEFAULT_AUTOROTATEUPRIGHTENUMVALID;
				bone->QueryUnsignedAttribute("autoRotateUprightEnumValId", &br.autoRotateUprightEnumValId);
				br.behavior = DEFAULT_BEHAVIOR;
				bone->QueryIntAttribute("behavior", &br.behavior);
				bone->QueryIntAttribute("behaviour", &br.behavior);	//I'M SO BRITISH
				br.burnGridSize = DEFAULT_BURNGRIDSIZE;	//Later in XML
				br.connectedGroupIdx = DEFAULT_CONNECTEDGROUPIDX;
				bone->QueryIntAttribute("connectedGroupIdx", &br.connectedGroupIdx);
				br.decayParticlesEnumValId = DEFAULT_DECAYPARTICLESENUMVALID;
				bone->QueryUnsignedAttribute("decayParticlesEnumValId", &br.decayParticlesEnumValId);
				br.explodeIgnitePiecesEnumValId = DEFAULT_EXPLODEIGNITEPIECESENUMVALID;
				bone->QueryUnsignedAttribute("explodeIgnitePiecesEnumValId", &br.explodeIgnitePiecesEnumValId);
				br.explodeIgnoreBurnTriggerEnumValId = DEFAULT_EXPLODEIGNOREBURNTRIGGERENUMVALID;
				bone->QueryUnsignedAttribute("explodeIgnoreBurnTriggerEnumValId", &br.explodeIgnoreBurnTriggerEnumValId);
				br.igniteParticlesEnumValId = DEFAULT_IGNITEPARTICLESENUMVALID;
				bone->QueryUnsignedAttribute("igniteParticlesEnumValId", &br.igniteParticlesEnumValId);
				br.instAshOnCollideEnumValId = DEFAULT_INSTASHONCOLLIDEENUMVALID;
				bone->QueryUnsignedAttribute("instAshOnCollideEnumValId", &br.instAshOnCollideEnumValId);
				br.postExplodeAshBreakMinAccelEnumValId = DEFAULT_POSTEXPLODEASHBREAKMINACCELENUMVALID;
				bone->QueryUnsignedAttribute("postExplodeAshBreakMinAccelEnumValId", &br.postExplodeAshBreakMinAccelEnumValId);
				br.postExplodeSplitTimeBaseEnumValId = DEFAULT_POSTEXPLODESPLITTIMEBASEENUMVALID;
				bone->QueryUnsignedAttribute("postExplodeSplitTimeBaseEnumValId", &br.postExplodeSplitTimeBaseEnumValId);
				br.postExplodeSplitTimeVarEnumValId = DEFAULT_POSTEXPLODESPLITTIMEVARENUMVALID;
				bone->QueryUnsignedAttribute("postExplodeSplitTimeVarEnumValId", &br.postExplodeSplitTimeVarEnumValId);
				br.shatterExpDoCamShakeEnumValId = DEFAULT_SHATTEREXPDOCAMSHAKEENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpDoCamShakeEnumValId", &br.shatterExpDoCamShakeEnumValId);
				br.shatterExpEffectEnumValId = DEFAULT_SHATTEREXPEFFECTENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpEffectEnumValId", &br.shatterExpEffectEnumValId);
				br.shatterExpFireAmountEnumValId = DEFAULT_SHATTEREXPFIREAMOUNTENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpFireAmountEnumValId", &br.shatterExpFireAmountEnumValId);
				br.shatterExpFireSpeedEnumValId = DEFAULT_SHATTEREXPFIRESPEEDENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpFireSpeedEnumValId", &br.shatterExpFireSpeedEnumValId);
				br.shatterExpForceEnumValId = DEFAULT_SHATTEREXPFORCEENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpForceEnumValId", &br.shatterExpForceEnumValId);
				br.shatterExpRadiusEnumValId = DEFAULT_SHATTEREXPRADIUSENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpRadiusEnumValId", &br.shatterExpRadiusEnumValId);
				br.shatterExpSoundEnumValId = DEFAULT_SHATTEREXPSOUNDENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpSoundEnumValId", &br.shatterExpSoundEnumValId);
				br.shatterExpTimeFactorEnumValId = DEFAULT_SHATTEREXPTIMEFACTORENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpTimeFactorEnumValId", &br.shatterExpTimeFactorEnumValId);
				br.shatterExpTimeHoldDownEnumValId = DEFAULT_SHATTEREXPTIMEHOLDDOWNENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpTimeHoldDownEnumValId", &br.shatterExpTimeHoldDownEnumValId);
				br.shatterExpTimeRampDownEnumValId = DEFAULT_SHATTEREXPTIMERAMPDOWNENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpTimeRampDownEnumValId", &br.shatterExpTimeRampDownEnumValId);
				br.shatterExpTimeRampUpEnumValId = DEFAULT_SHATTEREXPTIMERAMPUPENUMVALID;
				bone->QueryUnsignedAttribute("shatterExpTimeRampUpEnumValId", &br.shatterExpTimeRampUpEnumValId);
				br.smearAmountEnumValId = DEFAULT_SMEARAMOUNTENUMVALID;
				bone->QueryUnsignedAttribute("smearAmountEnumValId", &br.smearAmountEnumValId);
				br.splatParticlesEnumValId = DEFAULT_SPLATPARTICLESENUMVALID;
				bone->QueryUnsignedAttribute("splatParticlesEnumValId", &br.splatParticlesEnumValId);
				br.splitBrittleEnumValId = DEFAULT_SPLITBRITTLEENUMVALID;
				bone->QueryUnsignedAttribute("splitBrittleEnumValId", &br.splitBrittleEnumValId);
				br.splitDespawnEffectEnumValId = DEFAULT_SPLITDESPAWNEFFECTENUMVALID;
				bone->QueryUnsignedAttribute("splitDespawnEffectEnumValId", &br.splitDespawnEffectEnumValId);
				br.splitEffectEnumValId = DEFAULT_SPLITEFFECTENUMVALID;
				bone->QueryUnsignedAttribute("splitEffectEnumValId", &br.splitEffectEnumValId);
				br.splitSFXLargeEnumValId = DEFAULT_SPLITSFXLARGEENUMVALID;
				bone->QueryUnsignedAttribute("splitSFXLargeEnumValId", &br.splitSFXLargeEnumValId);
				br.splitSFXMediumEnumValId = DEFAULT_SPLITSFXMEDIUMENUMVALID;
				bone->QueryUnsignedAttribute("splitSFXMediumEnumValId", &br.splitSFXMediumEnumValId);
				br.splitSFXSmallEnumValId = DEFAULT_SPLITSFXSMALLENUMVALID;
				bone->QueryUnsignedAttribute("splitSFXSmallEnumValId", &br.splitSFXSmallEnumValId);
				br.splitThresholdEnumValId = DEFAULT_SPLITTHRESHOLDENUMVALID;
				bone->QueryUnsignedAttribute("splitThresholdEnumValId", &br.splitThresholdEnumValId);
				
				
				//Now read important stuff, erroring out if anything is missing
				if(bone->QueryUnsignedAttribute("id", &br.id) != XML_NO_ERROR)
				{
					cout << "Unable to read bone id from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryIntAttribute("animBlockIdx", &br.animBlockIdx) != XML_NO_ERROR)
				{
					cout << "Unable to read bone animBlockIdx from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				cVec = bone->Attribute("itemSpacePosition");
				if(cVec == NULL)
				{
					cout << "Unable to read bone itemSpacePosition in XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				br.itemSpacePosition = stringToVec2(cVec);
				cVec = bone->Attribute("burnBoundsMin");
				if(cVec == NULL)
				{
					cout << "Unable to read bone burnBoundsMin in XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				br.burnBoundsMin = stringToVec2(cVec);
				cVec = bone->Attribute("burnBoundsMax");
				if(cVec == NULL)
				{
					cout << "Unable to read bone burnBoundsMax in XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				br.burnBoundsMax = stringToVec2(cVec);
				if(bone->QueryUnsignedAttribute("igniteTimeEnumValId", &br.igniteTimeEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone igniteTimeEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("burnTimeEnumValId", &br.burnTimeEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone burnTimeEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("attackSpeedEnumValId", &br.attackSpeedEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone attackSpeedEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("attackAmountEnumValId", &br.attackAmountEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone attackAmountEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("decaySpeedEnumValId", &br.decaySpeedEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone decaySpeedEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("burnAmountEnumValId", &br.burnAmountEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone burnAmountEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("boneDensityEnumValId", &br.boneDensityEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone boneDensityEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("collideSoundEnumValId", &br.collideSoundEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone collideSoundEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("igniteSoundEnumValId", &br.igniteSoundEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone igniteSoundEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("frictionEnumValId", &br.frictionEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone frictionEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("restitutionEnumValId", &br.restitutionEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone restitutionEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("linearDampEnumValId", &br.linearDampEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone linearDampEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("angularDampEnumValId", &br.angularDampEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone angularDampEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("ashBreakMinAccelEnumValId", &br.ashBreakMinAccelEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone ashBreakMinAccelEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("ashBreakMaxAccelEnumValId", &br.ashBreakMaxAccelEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone ashBreakMaxAccelEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("stampBlackWhitePctEnumValId", &br.stampBlackWhitePctEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone stampBlackWhitePctEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("collideParticlesEnumValId", &br.collideParticlesEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone collideParticlesEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				if(bone->QueryUnsignedAttribute("mouseGrabSoundEnumValId", &br.mouseGrabSoundEnumValId) != XML_NO_ERROR)
				{
					cout << "Unable to read bone mouseGrabSoundEnumValId from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				
				//Read children shapes
				br.firstBoneMainShapeIdx = lBoneShapes.size();
				br.numBoneMainShapes = 0;
				for(XMLElement* shape = bone->FirstChildElement("shape"); shape != NULL; shape = shape->NextSiblingElement("shape"))
				{
					boneShapeRecord bsr;
					
					const char* cType = shape->Attribute("type");
					if(cType == NULL)
					{
						cout << "Error: Could not read bone shape type from XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					string sType = cType;
					if(sType == "circle")	//Circle shape
					{
						bsr.flags = TYPE_CIRCLE;
						bsr.numVerts = 2;
						for(int j = 0; j < 8; j++)
							bsr.verts[j].x = bsr.verts[j].y = 0.0f;
						XMLElement* center = shape->FirstChildElement("center");
						if(center == NULL)
						{
							cout << "Error: Could not read circle center from XML file " << ws2s(*i) << endl;
							delete doc; 
							return false;
						}
						readVec2(center, "pos", &(bsr.verts[0]));
						XMLElement* radius = shape->FirstChildElement("radius");
						if(radius == NULL)
						{
							cout << "Error: Could not read circle radius from XML file " << ws2s(*i) << endl;
							delete doc; 
							return false;
						}
						if((radius->QueryFloatAttribute("value", &(bsr.verts[1].x))) != XML_NO_ERROR)
						{
							cout << "Error: Could not read circle radius from XML file " << ws2s(*i) << ". Value malformed." << endl;
							delete doc; 
							return false;
						}						
					}
					else if(sType == "polygon")	//polygon shape
					{
						bsr.flags = TYPE_POLYGON;
						bsr.numVerts = 0;
						for(int i = 0; i < 8; i++)	//Set all verts to 0 to start off
							bsr.verts[i].x = bsr.verts[i].y = 0.0f;
						for(XMLElement* vert = shape->FirstChildElement("vert"); vert != NULL; vert = vert->NextSiblingElement("vert"))
						{
							readVec2(vert, "pos", &(bsr.verts[bsr.numVerts]));
							bsr.numVerts++;
						}
					}
					else
					{
						cout << "Unrecognized bone shape type " << sType << " in XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					
					lBoneShapes.push_back(bsr);
					br.numBoneMainShapes++;
				}
				
				//Read children parts
				br.firstPartsIdx = lBoneParts.size();
				br.numParts = 0;
				XMLElement* parts = bone->FirstChildElement("parts");
				if(parts != NULL)
				{
					for(XMLElement* part = parts->FirstChildElement("part"); part != NULL; part = part->NextSiblingElement("part"))
					{
						bonePartRecord bpr;
						if(part->QueryUnsignedAttribute("flags", &bpr.flags) != XML_NO_ERROR)
						{
							cout << "Error: Unable to read bone part flags from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						if(part->QueryUnsignedAttribute("texId", &bpr.texResId) != XML_NO_ERROR)
						{
							cout << "Error: Unable to read bone part texId from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						if(part->QueryUnsignedAttribute("normId", &bpr.normalMapResId) != XML_NO_ERROR)
						{
							cout << "Error: Unable to read bone part normId from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						if(part->QueryFloatAttribute("pupilRange", &bpr.pupilMoveRange) != XML_NO_ERROR)
						{
							cout << "Error: Unable to read bone part pupilRange from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						br.numParts++;
						lBoneParts.push_back(bpr);
					}
				}
				else
					br.firstPartsIdx = -1;
								
				//Read children parttree values
				br.firstPartTreeValIdx = lBonePartTreeValues.size();
				br.numPartTreeVals = 0;
				XMLElement* parttree = bone->FirstChildElement("parttree");
				if(parttree != NULL)
				{
					for(XMLElement* value = parttree->FirstChildElement("value"); value != NULL; value = value->NextSiblingElement("value"))
					{
						i32 val;
						if(value->QueryIntAttribute("val", &val) != XML_NO_ERROR)
						{
							cout << "Error: Unable to read bone part tree value from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						lBonePartTreeValues.push_back(val);
						br.numPartTreeVals++;
					}
				}
				else
					br.firstPartTreeValIdx = -1;
				
				//Read child burngrid
				br.firstBurnUsedIdx = lBurnGrid.size();
				XMLElement* burngrid = bone->FirstChildElement("burngrid");
				if(burngrid != NULL)
				{
					burngrid->QueryFloatAttribute("size", &br.burnGridSize);	//Don't care if this fails, default value of 10.0f was set earlier
					if(burngrid->QueryIntAttribute("width", &br.burnGridWidth) != XML_NO_ERROR)
					{
						cout << "Error: Unable to read burngrid width from XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					if(burngrid->QueryIntAttribute("height", &br.burnGridHeight) != XML_NO_ERROR)
					{
						cout << "Error: Unable to read burngrid height from XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					if(burngrid->QueryIntAttribute("firstpos", &br.firstBurnUsedIdx) != XML_NO_ERROR)
					{
						cout << "Error: Unable to read burngrid firstpos from XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					const char* grid = burngrid->Attribute("grid");
					if(grid == NULL)
					{
						cout << "Error: Unable to read burngrid grid from XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					if(strlen(grid) != br.burnGridWidth * br.burnGridHeight)	//Grid isn't correct size
					{
						cout << "Error: burngrid attribute \"grid\" is not of width*height length in XML file " << ws2s(*i) << endl;
						delete doc;
						return false;
					}
					for(int j = 0; j < br.burnGridWidth*br.burnGridHeight; j++)
					{
						if(grid[j] == '0')
							lBurnGrid.push_back(0);
						else
							lBurnGrid.push_back(1);
					}
				}
				else
				{
					cout << "Error: Unable to read bone burngrid from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				
				//Read region mappings if here
				br.firstRgnCellIdx = lBoneRgnCells.size();
				br.numRgnCells = 0;
				XMLElement* regions = bone->FirstChildElement("regions");
				if(regions != NULL)
				{
					for(XMLElement* region = regions->FirstChildElement("regionmapping"); region != NULL; region = region->NextSiblingElement("regionmapping"))
					{
						boneGridCellMappingRegion bgcmr;
						if(region->QueryIntAttribute("uid", &bgcmr.regionUID) != XML_NO_ERROR)
						{	
							cout << "Error: Unable to read bone regionmapping uid from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}
						if(region->QueryIntAttribute("index", &bgcmr.burnGridCellIdx) != XML_NO_ERROR)
						{	
							cout << "Error: Unable to read bone regionmapping index from XML file " << ws2s(*i) << endl;
							delete doc;
							return false;
						}						
						br.numRgnCells++;
						lBoneRgnCells.push_back(bgcmr);
					}
				}
				else
				{
					//Don't care, but set to -1 for no cells
					br.firstRgnCellIdx = -1;
				}
				
				//Read transforms
				XMLElement* transforms = bone->FirstChildElement("transforms");
				if(transforms == NULL)
				{
					cout << "Error: Unable to read bone transforms from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				ReadTransforms(transforms, &br.animBlockTransform);
				
				sr.numBones++;
				lBones.push_back(br);
			}
			
			lSkels.push_back(sr);
		}
		vSkeletons.push_back(lSkels);
		vJoints.push_back(lJoints);
		vBones.push_back(lBones);
		vBoneShapes.push_back(lBoneShapes);
		vBoneParts.push_back(lBoneParts);
		vBonePartTreeValues.push_back(lBonePartTreeValues);
		vBoneRgnCells.push_back(lBoneRgnCells);
		vBurnGrid.push_back(lBurnGrid);
		
		//TODO Read in string table
		StringTableHeader sth;
		sth.numStrings = sth.numPointers = 0;
		list<StringTableEntry> lStringEntries;
		list<StringPointerEntry> lStringPointers;
		list<char> lStrings;
		
		//String Table Header
		//i32 numStrings;
		//i32 numPointers;
		
		//String Table Entry
		//i32 pointerIndex;
		//i32 pointerCount;
		
		//String Pointer Entry
		//u32 languageId;
		//i32 offset;
		
		//Read in ID string
		StringTableEntry idEntry;
		idEntry.pointerIndex = lStringPointers.size();
		idEntry.pointerCount = 0;
		idh.itemIdStrId = sth.numStrings;
		XMLElement* id = elem->FirstChildElement("id");	//Get ID string
		if(id == NULL)
		{
			cout << "Unable to read item ID string from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* idstring = id->FirstChildElement("string"); idstring != NULL; idstring = idstring->NextSiblingElement("string"))
		{
			StringPointerEntry spe;
			const char* lang = idstring->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to read item ID string's language from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			spe.languageId = toLangID(s2ws(lang));
			spe.offset = lStrings.size();
			lStringPointers.push_back(spe);
			idEntry.pointerCount++;
			sth.numPointers++;
			
			//Read in string data
			const char* data = idstring->Attribute("data");
			if(data == NULL)
			{
				cout << "Unable to read item ID string's data from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			for(int j = 0; j < strlen(data); j++)
				lStrings.push_back(data[j]);
			lStrings.push_back('\0');	//Be sure to append null character, as well
		}
		lStringEntries.push_back(idEntry);
		sth.numStrings++;
		
		//Read in name string
		StringTableEntry nameEntry;
		nameEntry.pointerIndex = lStringPointers.size();
		nameEntry.pointerCount = 0;
		idh.name.key = sth.numStrings;
		XMLElement* namestr = elem->FirstChildElement("name");	//Get ID string
		if(namestr == NULL)
		{
			cout << "Unable to read item name string from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		if(namestr->QueryUnsignedAttribute("strid", &idh.name.id) != XML_NO_ERROR)
		{
			cout << "Unable to read item name strid XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* namestring = namestr->FirstChildElement("string"); namestring != NULL; namestring = namestring->NextSiblingElement("string"))
		{
			StringPointerEntry spe;
			const char* lang = namestring->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to read item name string's language from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			spe.languageId = toLangID(s2ws(lang));
			spe.offset = lStrings.size();
			lStringPointers.push_back(spe);
			nameEntry.pointerCount++;
			sth.numPointers++;
			
			//Read in string data
			const char* data = namestring->Attribute("data");
			if(data == NULL)
			{
				cout << "Unable to read item name string's data from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			for(int j = 0; j < strlen(data); j++)
				lStrings.push_back(data[j]);
			lStrings.push_back('\0');	//Be sure to append null character, as well
		}
		lStringEntries.push_back(nameEntry);
		sth.numStrings++;
		
		//Read in description string
		StringTableEntry descEntry;
		descEntry.pointerIndex = lStringPointers.size();
		descEntry.pointerCount = 0;
		idh.desc.key = sth.numStrings;
		XMLElement* descstr = elem->FirstChildElement("description");	//Get ID string
		if(descstr == NULL)
		{
			cout << "Unable to read item description string from XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		if(descstr->QueryUnsignedAttribute("strid", &idh.desc.id) != XML_NO_ERROR)
		{
			cout << "Unable to read item description strid XML file " << ws2s(*i) << endl;
			delete doc;
			return false;
		}
		for(XMLElement* descstring = descstr->FirstChildElement("string"); descstring != NULL; descstring = descstring->NextSiblingElement("string"))
		{
			StringPointerEntry spe;
			const char* lang = descstring->Attribute("lang");
			if(lang == NULL)
			{
				cout << "Unable to read item description string's language from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			spe.languageId = toLangID(s2ws(lang));
			spe.offset = lStrings.size();
			lStringPointers.push_back(spe);
			descEntry.pointerCount++;
			sth.numPointers++;
			
			//Read in string data
			const char* data = descstring->Attribute("data");
			if(data == NULL)
			{
				cout << "Unable to read item description string's data from XML file " << ws2s(*i) << endl;
				delete doc;
				return false;
			}
			for(int j = 0; j < strlen(data); j++)
				lStrings.push_back(data[j]);
			lStrings.push_back('\0');	//Be sure to append null character, as well
		}
		lStringEntries.push_back(descEntry);
		sth.numStrings++;
		
		//Read in other string data
		for(XMLElement* text = elem->FirstChildElement("text"); text != NULL; text = text->NextSiblingElement("text"))
		{
			StringTableEntry ste;
			ste.pointerIndex = lStringPointers.size();
			ste.pointerCount = 0;
			for(XMLElement* textstring = text->FirstChildElement("string"); textstring != NULL; textstring = textstring->NextSiblingElement("string"))
			{
				StringPointerEntry spe;
				const char* lang = textstring->Attribute("lang");
				if(lang == NULL)
				{
					cout << "Unable to read text string's language from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				spe.languageId = toLangID(s2ws(lang));
				spe.offset = lStrings.size();
				lStringPointers.push_back(spe);
				ste.pointerCount++;
				sth.numPointers++;
				
				//Read in string data
				const char* data = textstring->Attribute("data");
				if(data == NULL)
				{
					cout << "Unable to read text string's data from XML file " << ws2s(*i) << endl;
					delete doc;
					return false;
				}
				for(int j = 0; j < strlen(data); j++)
					lStrings.push_back(data[j]);
				lStrings.push_back('\0');	//Be sure to append null character, as well
			}
			lStringEntries.push_back(ste);
			sth.numStrings++;
		}
		
		vStringTableHeaders.push_back(sth);
		vlStringTables.push_back(lStringEntries);
		vlStringPointers.push_back(lStringPointers);
		vlStrings.push_back(lStrings);
		
		idh.skels.count = lSkels.size();
		idh.skels.offset = sizeof(itemDataHeader);
		idh.joints.count = lJoints.size();
		idh.joints.offset = idh.skels.offset + idh.skels.count * sizeof(skelsRecord);
		idh.bones.count = lBones.size();
		idh.bones.offset = idh.joints.offset + idh.joints.count * sizeof(jointRecord);
		idh.boneShapes.count = lBoneShapes.size();
		idh.boneShapes.offset = idh.bones.offset + idh.bones.count * sizeof(boneRecord);
		idh.boneParts.count = lBoneParts.size();
		idh.boneParts.offset = idh.boneShapes.offset + idh.boneParts.count * sizeof(boneShapeRecord);
		idh.bonePartTreeVals.count = lBonePartTreeValues.size();
		idh.bonePartTreeVals.offset = idh.boneParts.offset + idh.boneParts.count * sizeof(bonePartRecord);
		idh.rgnCells.count = lBoneRgnCells.size();
		idh.rgnCells.offset = idh.bonePartTreeVals.offset + idh.bonePartTreeVals.count * sizeof(i32);
		idh.stringTableBytes.count = sizeof(StringTableHeader) + sizeof(StringTableEntry) * lStringEntries.size() + sizeof(StringPointerEntry) * lStringPointers.size() + lStrings.size();
		idh.stringTableBytes.offset = idh.rgnCells.offset + idh.rgnCells.count * sizeof(boneGridCellMappingRegion);
		idh.burnGridUsedDataBytes.count = lBurnGrid.size();
		idh.burnGridUsedDataBytes.offset = idh.stringTableBytes.offset + idh.stringTableBytes.count;
		binDataRunningTally += idh.burnGridUsedDataBytes.offset + idh.burnGridUsedDataBytes.count;	//Update our running tally of the size of the binary data
			
		lItemData.push_back(idh);
		lItemManifests.push_back(imr);
		
		//And we're done with this XML file
		delete doc;
	}
	
	//Write everything out to the itemmanifest file
	wstring sFilename = cFilename;
	//sFilename += TEXT(".derp");	//TODO: Replace original file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open file " << cFilename << " for writing." << endl;
		return false;
	}
	
	//Write our itemManifestHeader
	itemManifestHeader imh;
	imh.itemsManifest.count = lItemManifests.size();
	imh.itemsManifest.offset = sizeof(itemManifestHeader);
	imh.normalDeps.count = lNormalDeps.size();
	imh.normalDeps.offset = imh.itemsManifest.offset + imh.itemsManifest.count * sizeof(itemManifestRecord);
	imh.soundDeps.count = lSoundDeps.size();
	imh.soundDeps.offset = imh.normalDeps.offset + imh.normalDeps.count * sizeof(normalDependency);
	imh.effectDeps.count = lEffectDeps.size();
	imh.effectDeps.offset = imh.soundDeps.offset + imh.soundDeps.count * sizeof(soundDependency);
	imh.itemDeps.count = lItemDeps.size();
	imh.itemDeps.offset = imh.effectDeps.offset + imh.effectDeps.count * sizeof(effectDependency);
	imh.itemsBinDataBytes.count = binDataRunningTally;
	imh.itemsBinDataBytes.offset = imh.itemDeps.offset + imh.itemDeps.count * sizeof(itemDependency);
	fwrite(&imh, 1, sizeof(itemManifestHeader), f);
	
	//Write our itemManifestRecords
	for(list<itemManifestRecord>::iterator i = lItemManifests.begin(); i != lItemManifests.end(); i++)
		fwrite(&(*i), 1, sizeof(itemManifestRecord), f);
	
	//Write our normalDependencies
	for(list<normalDependency>::iterator i = lNormalDeps.begin(); i != lNormalDeps.end(); i++)
		fwrite(&(*i), 1, sizeof(normalDependency), f);
	
	//Write our soundDependencies
	for(list<soundDependency>::iterator i = lSoundDeps.begin(); i != lSoundDeps.end(); i++)
		fwrite(&(*i), 1, sizeof(soundDependency), f);
	
	//Write our effectDependencies
	for(list<effectDependency>::iterator i = lEffectDeps.begin(); i != lEffectDeps.end(); i++)
		fwrite(&(*i), 1, sizeof(effectDependency), f);
	
	//Write our itemDependencies
	for(list<itemDependency>::iterator i = lItemDeps.begin(); i != lItemDeps.end(); i++)
		fwrite(&(*i), 1, sizeof(itemDependency), f);
	
	//Write binary stuff
	int iCurItem = 0;
	for(vector<itemDataHeader>::iterator i = lItemData.begin(); i != lItemData.end(); i++, iCurItem++)
	{
		//Item data header
		fwrite(&(*i), 1, sizeof(itemDataHeader), f);
		
		//Skeletons for this item
		for(list<skelsRecord>::iterator j = vSkeletons[iCurItem].begin(); j != vSkeletons[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(skelsRecord), f);
		
		//Joints for this skeleton
		for(list<jointRecord>::iterator j = vJoints[iCurItem].begin(); j != vJoints[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(jointRecord), f);
		
		//Bones for this skeleton
		for(list<boneRecord>::iterator j = vBones[iCurItem].begin(); j != vBones[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(boneRecord), f);
			
		//Shapes for this bone
		for(list<boneShapeRecord>::iterator j = vBoneShapes[iCurItem].begin(); j != vBoneShapes[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(boneShapeRecord), f);
		
		//Parts for this bone
		for(list<bonePartRecord>::iterator j = vBoneParts[iCurItem].begin(); j != vBoneParts[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(bonePartRecord), f);
		
		//Tree values for this bone
		for(list<i32>::iterator j = vBonePartTreeValues[iCurItem].begin(); j != vBonePartTreeValues[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(i32), f);
		
		//Cell regions for this bone
		for(list<boneGridCellMappingRegion>::iterator j = vBoneRgnCells[iCurItem].begin(); j != vBoneRgnCells[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(boneGridCellMappingRegion), f);
		
		//String table header
		fwrite(&(vStringTableHeaders[iCurItem]), 1, sizeof(StringTableHeader), f);
		
		//String table entries
		for(list<StringTableEntry>::iterator j = vlStringTables[iCurItem].begin(); j != vlStringTables[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(StringTableEntry), f);
		
		//String pointer entries
		for(list<StringPointerEntry>::iterator j = vlStringPointers[iCurItem].begin(); j != vlStringPointers[iCurItem].end(); j++)
			fwrite(&(*j), 1, sizeof(StringPointerEntry), f);
		
		//Strings
		for(list<char>::iterator j = vlStrings[iCurItem].begin(); j != vlStrings[iCurItem].end(); j++)
			fwrite(&(*j), 1, 1, f);
		
		//Burn grid
		for(list<byte>::iterator j = vBurnGrid[iCurItem].begin(); j != vBurnGrid[iCurItem].end(); j++)
			fwrite(&(*j), 1, 1, f);

	}
	
	fclose(f);
	return true;
}


















