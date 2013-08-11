#include "pakDataTypes.h"

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
#define DEFAULT_DESCKEY					2
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
#define DEFAULT_ITEMIDSTRID				0
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
#define DEFAULT_NAMEKEY					1
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

wstring toLangString(u32 languageId)	//Convert a language ID to a string (for example, 0x656E becomes "en")
{
	wstring s;
	s.push_back(languageId >> 8);
	s.push_back(languageId & 0xFF);
	return s;
}

wstring getNameFromAnim(wstring sAnimName)
{
	sAnimName.erase(sAnimName.rfind(TEXT(".anim.xml")));	//Erase the ending off the string
	size_t start = sAnimName.find_last_of('/') + 1;	//Find last forward slash
	sAnimName.erase(0,start);						//Erase everything up to and including this last slash
	return sAnimName;								//Done
}

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
		mItemNames[imr.itemId] = getNameFromAnim(getName(imr.animResId));
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
	vector< vector<wchar_t> > vStrings;
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
		//cout << "String size: " << iStringSize << ", StringTableBytes count: " << idh.stringTableBytes.count << endl;
	
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
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		#ifdef SPLIT_XML_FILES
		XMLDocument* doc = new XMLDocument;
		//XMLElement* root = doc->NewElement("itemmanifest");	//Create the root element
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
		elem->SetAttribute("id", ws2s(mItemNames[i->itemId]).c_str());
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
		//TODO: binDataOffsetBytes stuff
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
			elem3->SetAttribute("id", ws2s(getSoundName(vSoundDependencies[j].soundResId)).c_str());
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstEffectDepends; j < i->firstEffectDepends + i->numEffectDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("effect");
			elem3->SetAttribute("id", ws2s(getName(vEffectDependencies[j].effectResId)).c_str());
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
		elem2->SetAttribute("descid", vItemDataHeaders[iCurItemData].desc.id);
		if(vItemDataHeaders[iCurItemData].desc.key != DEFAULT_DESCKEY)
			elem2->SetAttribute("desckey", vItemDataHeaders[iCurItemData].desc.key);
		if(vItemDataHeaders[iCurItemData].enableFreezePostAnim != DEFAULT_ENABLEFREEZEPOSTANIM)
			elem2->SetAttribute("enableFreezePostAnim", vItemDataHeaders[iCurItemData].enableFreezePostAnim);
		if(vItemDataHeaders[iCurItemData].enableHFlip != DEFAULT_ENABLEHFLIP)
			elem2->SetAttribute("enableHFlip", vItemDataHeaders[iCurItemData].enableHFlip);
		if(vItemDataHeaders[iCurItemData].floorWallShadow != DEFAULT_FLOORWALLSHADOW)
			elem2->SetAttribute("floorWallShadow", vItemDataHeaders[iCurItemData].floorWallShadow);
		if(vItemDataHeaders[iCurItemData].freezeOnCollide != DEFAULT_FREEZEONCOLLIDE)
			elem2->SetAttribute("freezeOnCollide", vItemDataHeaders[iCurItemData].freezeOnCollide);
		elem2->SetAttribute("iconAnimBoundsMaxx", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.x);
		elem2->SetAttribute("iconAnimBoundsMaxy", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.y);
		elem2->SetAttribute("iconAnimBoundsMinx", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.x);
		elem2->SetAttribute("iconAnimBoundsMiny", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.y);
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
		if(vItemDataHeaders[iCurItemData].itemIdStrId != DEFAULT_ITEMIDSTRID)
			elem2->SetAttribute("itemIdStrId", vItemDataHeaders[iCurItemData].itemIdStrId);
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
		elem2->SetAttribute("nameid", vItemDataHeaders[iCurItemData].name.id);
		if(vItemDataHeaders[iCurItemData].name.key != DEFAULT_NAMEKEY)
			elem2->SetAttribute("namekey", vItemDataHeaders[iCurItemData].name.key);
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
		XMLElement* elem3;// = doc->NewElement("skeleton");
		for(list<skelsRecord>::iterator j = vSkeletonRecords[iCurItemData].begin(); j != vSkeletonRecords[iCurItemData].end(); j++)
		{
			elem3 = doc->NewElement("skeleton");
			elem3->SetAttribute("burnExport", j->burnExport);
			elem3->SetAttribute("selectWeight", j->selectWeight);
			elem3->SetAttribute("hasAnimThresh", j->hasAnimThresh);
			elem3->SetAttribute("animThresh", j->animThresh);
			elem3->SetAttribute("animExportStrId", j->animExportStrId);
			elem3->SetAttribute("animBoundsMinx", j->animBoundsMin.x);
			elem3->SetAttribute("animBoundsMiny", j->animBoundsMin.y);
			elem3->SetAttribute("animBoundsMaxx", j->animBoundsMax.x);
			elem3->SetAttribute("animBoundsMaxy", j->animBoundsMax.y);
			
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
				elem4->SetAttribute("modelSpacePosx", vJointRecords[iCurItemData][k].modelSpacePos.x);
				elem4->SetAttribute("modelSpacePosy", vJointRecords[iCurItemData][k].modelSpacePos.y);
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
				elem4->SetAttribute("animBlockTransform_11", vBoneRecords[iCurItemData][k].animBlockTransform._11);
				elem4->SetAttribute("animBlockTransform_12", vBoneRecords[iCurItemData][k].animBlockTransform._12);
				elem4->SetAttribute("animBlockTransform_13", vBoneRecords[iCurItemData][k].animBlockTransform._13);
				elem4->SetAttribute("animBlockTransform_21", vBoneRecords[iCurItemData][k].animBlockTransform._21);
				elem4->SetAttribute("animBlockTransform_22", vBoneRecords[iCurItemData][k].animBlockTransform._22);
				elem4->SetAttribute("animBlockTransform_23", vBoneRecords[iCurItemData][k].animBlockTransform._23);
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
				elem4->SetAttribute("burnBoundsMaxx", vBoneRecords[iCurItemData][k].burnBoundsMax.x);
				elem4->SetAttribute("burnBoundsMaxy", vBoneRecords[iCurItemData][k].burnBoundsMax.y);
				elem4->SetAttribute("burnBoundsMinx", vBoneRecords[iCurItemData][k].burnBoundsMin.x);
				elem4->SetAttribute("burnBoundsMiny", vBoneRecords[iCurItemData][k].burnBoundsMin.y);
				//elem4->SetAttribute("burnGridHeight", vBoneRecords[iCurItemData][k].burnGridHeight);
				if(vBoneRecords[iCurItemData][k].burnGridSize != DEFAULT_BURNGRIDSIZE)
					elem4->SetAttribute("burnGridSize", vBoneRecords[iCurItemData][k].burnGridSize);
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
				elem4->SetAttribute("firstBurnUsedIdx", vBoneRecords[iCurItemData][k].firstBurnUsedIdx);
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
				elem4->SetAttribute("itemSpacePositionx", vBoneRecords[iCurItemData][k].itemSpacePosition.x);
				elem4->SetAttribute("itemSpacePositiony", vBoneRecords[iCurItemData][k].itemSpacePosition.y);
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
							
							elem6->SetAttribute("x", vBoneShapes[iCurItemData][m].verts[m].x);
							elem6->SetAttribute("y", vBoneShapes[iCurItemData][m].verts[m].y);
							
							elem5->InsertEndChild(elem6);
						}
					}
					else if(vBoneShapes[iCurItemData][l].flags == TYPE_CIRCLE)	//Circle shape
					{
						elem5->SetAttribute("type", "circle");
						//Write vertices for this shape
						XMLElement* elem6;
						elem6 = doc->NewElement("center");
						elem6->SetAttribute("x", vBoneShapes[iCurItemData][l].verts[0].x);
						elem6->SetAttribute("y", vBoneShapes[iCurItemData][l].verts[0].y);
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
					elem5->SetAttribute("width", vBoneRecords[iCurItemData][k].burnGridWidth);
					elem5->SetAttribute("height", vBoneRecords[iCurItemData][k].burnGridHeight);
					wstring s;
					for(int l = 0; l < vBoneRecords[iCurItemData][k].burnGridWidth * vBoneRecords[iCurItemData][k].burnGridHeight; l++)
					{
						if(vBurnGrid[iCurItemData][l])
							s.push_back(L'1');
						else
							s.push_back(L'0');
					}
					elem5->SetAttribute("grid", ws2s(s).c_str());
					elem4->InsertEndChild(elem5);
				}
				
				//DEBUG Now loop back through here and pull data from it all
				//for(const XMLAttribute* att = elem4->FirstAttribute(); att != NULL; att = att->Next())
				//	mOccurrences[att->Name()].push_back(att->Value());
			}
			
			elem2->InsertEndChild(elem3);
		}
		
		//Add string table
		for(int l = 0; l < vStringTableEntries[iCurItemData].size(); l++)
		{
			elem3 = doc->NewElement("text");
			for(int m = vStringTableEntries[iCurItemData][l].pointerIndex; m < vStringTableEntries[iCurItemData][l].pointerIndex + vStringTableEntries[iCurItemData][l].pointerCount; m++)
			{
				XMLElement* elem6 = doc->NewElement("string");
				elem6->SetAttribute("lang", ws2s(toLangString(vStringPointerEntries[iCurItemData][m].languageId)).c_str());	//TODO to actual language code (like "en" or such, as it is in dat file)
				elem6->SetAttribute("data", ws2s(&(vStrings[iCurItemData].data()[vStringPointerEntries[iCurItemData][m].offset])).c_str());
				elem3->InsertEndChild(elem6);
			}
			elem2->InsertEndChild(elem3);
		}
		
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
		
		//TODO Read stuff
		
		
		
		
		
		
		
		
		delete doc;
	}
	
	/*
	//Read in all child elements
	list<itemManifestRecord> lItemManifests;
	list<normalDependency> lNormalDeps;
	list<soundDependency> lSoundDeps;
	list<effectDependency> lEffectDeps;
	list<itemDependency> lItemDeps;
	for(XMLElement* elem = root->FirstChildElement("itemrecord"); elem != NULL; elem = elem->NextSiblingElement("itemrecord"))
	{
		itemManifestRecord imr;
		imr.itemId = imr.animResId = imr.recentlyModifiedRank = imr.firstNormalDepends = imr.numNormalDepends = imr.firstSoundDepends = imr.numSoundDepends = imr.firstEffectDepends = imr.numEffectDepends = imr.firstItemDepends = imr.numItemDepends = imr.catalogIconColorItemTexResId = imr.catalogIconColorBGTexResId = imr.catalogIconGreyBGTexResId = imr.binDataOffsetBytes = 0;	//To supress compiler warnings
		const char* id = elem->Attribute("id");
		if(id == NULL)
		{
			cout << "Error: Unable to get id of XML element in file " << ws2s(sXMLFile) << endl;
			delete doc;
			return false;
		}
		imr.itemId = hash(s2ws(id)); //TODO ????
		imr.recentlyModifiedRank = 1;	//Because why not
		
		//get all the XML children of this
		for(XMLElement* elem2 = elem->FirstChildElement(); elem2 != NULL; elem2 = elem2->NextSiblingElement())
		{
			wstring sName = s2ws(elem2->Name());
			if(sName == TEXT("animresid"))
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for animresid in file " << ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				imr.animResId = getResID(s2ws(cTemp));
			}
			else if(sName == TEXT("recentlymodifiedrank"))
			{
				//Completely ignore //TODO Remove test case
			}
			else if(sName == TEXT("coloritemicon"))
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for coloritemicon in file " << ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				imr.catalogIconColorItemTexResId = getResID(s2ws(cTemp));
			}
			else if(sName == TEXT("colorbgicon"))
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for colorbgicon in file " << ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				imr.catalogIconColorBGTexResId = getResID(s2ws(cTemp));
			}
			else if(sName == TEXT("greybgicon"))
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for greybgicon in file " << ws2s(sXMLFile) << endl;
					delete doc;
					return false;
				}
				imr.catalogIconGreyBGTexResId = getResID(s2ws(cTemp));
			}
			//TODO: binDataOffsetBytes
			else if(sName == TEXT("depends"))
			{
				//Store offsets for these dependencies
				imr.firstNormalDepends = lNormalDeps.size();
				imr.firstSoundDepends = lSoundDeps.size();
				imr.firstEffectDepends = lEffectDeps.size();
				imr.firstItemDepends = lItemDeps.size();
				imr.numNormalDepends = 0;
				imr.numSoundDepends = 0;
				imr.numEffectDepends = 0;
				imr.numItemDepends = 0;
				//Get all child dependencies
				for(XMLElement* elem3 = elem2->FirstChildElement(); elem3 != NULL; elem3 = elem3->NextSiblingElement())
				{
					wstring sDependName = s2ws(elem3->Name());
					if(sDependName == TEXT("normal"))
					{
						const char* cTemp = elem3->Attribute("filename");
						if(cTemp == NULL)
						{
							cout << "Error: Missing filename for normal dependency in file " << ws2s(sXMLFile) << endl;
							delete doc;
							return false;
						}
						normalDependency nd;
						nd.normalTexResId = getResID(s2ws(cTemp));
						lNormalDeps.push_back(nd);
						imr.numNormalDepends++;
					}
					else if(sDependName == TEXT("sound"))
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for sound dependency in file " << ws2s(sXMLFile) << endl;
							delete doc;
							return false;
						}
						soundDependency sd;
						sd.soundResId = getSoundId(s2ws(cTemp));
						lSoundDeps.push_back(sd);
						imr.numSoundDepends++;
					}
					else if(sDependName == TEXT("effect"))
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for effect dependency in file " << ws2s(sXMLFile) << endl;
							delete doc;
							return false;
						}
						effectDependency ed;
						ed.effectResId = getResID(s2ws(cTemp));
						lEffectDeps.push_back(ed);
						imr.numEffectDepends++;
					}
					else if(sDependName == TEXT("item"))
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for item dependency in file " << ws2s(sXMLFile) << endl;
							delete doc;
							return false;
						}
						itemDependency id;
						id.itemResId = hash(s2ws(cTemp));
						lItemDeps.push_back(id);
						imr.numItemDepends++;
					}
					else if(sDependName == TEXT(""))
						cout << "Warning: Empty element name for depends in XML file " << ws2s(sXMLFile) << endl;
					else
						cout << "Warning: Unknown name for dependency: " << ws2s(sDependName) << " in XML file " << ws2s(sXMLFile) << ". Ignoring..." << endl;
				}
			}
			else if(sName == TEXT(""))
				cout << "Warning: XML element missing name in file " << ws2s(sXMLFile) << ". Ignoring... " << endl;
			else
				cout << "Warning: Unknown XML element name: " << ws2s(sName) << " in XML file " << ws2s(sXMLFile) << ". Ignoring..." << endl;
		}
		lItemManifests.push_back(imr);
	}
	//Now we have all the item stuff properly in our lists
	delete doc;	//We're done with this
	
	//TODO Finish parsing XML binDataOffsetBytes stuff and write to file
	itemManifestHeader imh;
	size_t curOffset = sizeof(itemManifestHeader);
	imh.itemsManifest.count = lItemManifests.size();
	imh.itemsManifest.offset = curOffset;
	curOffset += sizeof(itemManifestRecord) * lItemManifests.size();
	imh.normalDeps.count = lNormalDeps.size();
	imh.normalDeps.offset = curOffset;
	curOffset += sizeof(normalDependency) * lNormalDeps.size();
	imh.soundDeps.count = lSoundDeps.size();
	imh.soundDeps.offset = curOffset;
	curOffset += sizeof(soundDependency) * lSoundDeps.size();
	imh.effectDeps.count = lEffectDeps.size();
	imh.effectDeps.offset = curOffset;
	curOffset += sizeof(effectDependency) * lEffectDeps.size();
	imh.itemDeps.count = lItemDeps.size();
	imh.itemDeps.offset = curOffset;
	curOffset += sizeof(itemDependency) * lItemDeps.size();
	imh.itemsBinDataBytes.count = 0;	//TODO
	imh.itemsBinDataBytes.offset = curOffset;
	
	//Open output file
	wstring sFilename = cFilename;
	sFilename += TEXT(".derp");	//TODO: Replace original file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open file " << cFilename << " for writing." << endl;
		return false;
	}
	
	//Write our itemManifestHeader
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
	
	//TODO Write our binDataOffsetBytes stuff
	
	fclose(f);*/
	return true;
}


















