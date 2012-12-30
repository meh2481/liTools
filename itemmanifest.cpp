#include "pakDataTypes.h"

//Defaults for item XML data
#define	DEFAULT_ABS_POSITION			0
#define DEFAULT_DIRLIGHT				1
#define DEFAULT_EXPLODESTREAKS			1
#define DEFAULT_ANIMTHRESHOLD			0
#define DEFAULT_BURNSLOWSANIM			1
#define DEFAULT_CANGETPLAGUE			1
#define DEFAULT_COLLIDEENVIRONMENT		1
#define DEFAULT_COLLIDEITEMS			1
#define DEFAULT_COSTSTAMPS				0
#define DEFAULT_DESCKEY					2
#define DEFAULT_ENABLEFREEZEPOSTANIM	1
#define DEFAULT_ENABLEHFLIP				1
#define DEFAULT_ENABLEFLOORWALLSHADOW	1
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
#define DEFAULT_ULISTED					0
#define DEFAULT_VALUESTAMPS				0

string getNameFromAnim(string sAnimName)
{
	sAnimName.erase(sAnimName.rfind(".anim.xml"));	//Erase the ending off the string
	size_t start = sAnimName.find_last_of('/') + 1;	//Find last forward slash
	sAnimName.erase(0,start);						//Erase everything up to and including this last slash
	return sAnimName;								//Done
}

bool itemManifestToXML(const char* cFilename)
{
	//Open the file
	FILE* f = fopen(cFilename, "rb");
	if(f == NULL)
	{
		cout << "Error: could not open " << cFilename << " for reading." << endl;
		return false;
	}
	
	//Read in the manifest
	itemManifestHeader imh;
	if(fread(&imh, 1, sizeof(itemManifestHeader), f) != sizeof(itemManifestHeader))
	{
		cout << "Error: unable to read itemManifestHeader from file " << cFilename << endl;
		fclose(f);
		return false;
	}
	
	//Read in the itemManifestRecords
	list<itemManifestRecord> lManifestRecords;
	map<u32, string> mItemNames;
	fseek(f, imh.itemsManifest.offset, SEEK_SET);
	for(int i = 0; i < imh.itemsManifest.count; i++)
	{
		itemManifestRecord imr;
		if(fread(&imr, 1, sizeof(itemManifestRecord), f) != sizeof(itemManifestRecord))
		{
			cout << "Error: unable to read itemManifestRecord from file " << cFilename << endl;
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
			cout << "Error: unable to read normalDependency from file " << cFilename << endl;
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
			cout << "Error: unable to read soundDependency from file " << cFilename << endl;
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
			cout << "Error: unable to read effectDependency from file " << cFilename << endl;
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
			cout << "Error: unable to read itemDependency from file " << cFilename << endl;
			fclose(f);
			return false;
		}
		vItemDependencies.push_back(id);
	}
	
	//Read in the item data
	vector<itemDataHeader> vItemDataHeaders;
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		fseek(f, imh.itemsBinDataBytes.offset + i->binDataOffsetBytes, SEEK_SET);	//Seek to this position to read
		itemDataHeader idh;
		if(fread(&idh, 1, sizeof(itemDataHeader), f) != sizeof(itemDataHeader))
		{
			cout << "Error: Unable to read itemDataHeader from file " << cFilename << endl;
			fclose(f); 
			return false;
		}
		vItemDataHeaders.push_back(idh);
	}
	
	//TODO Read rest of item data
	
	fclose(f);
	
	//Now, open up the XML file and put all the data into it
	string sFilename = cFilename;
	sFilename += ".xml";
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("itemmanifest");	//Create the root element
	int iCurItemData = 0;
	
	//DEBUG: See how often each value occurs and see if there's a resonable default
	map<string, list<string> > mOccurrences;
	//
	
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		XMLElement* elem = doc->NewElement("itemrecord");
		elem->SetAttribute("id", mItemNames[i->itemId].c_str());
		XMLElement* elem2 = doc->NewElement("animresid");
		elem2->SetAttribute("filename", getName(i->animResId));
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("recentlymodifiedrank");	//TODO Ignore this
		elem2->SetAttribute("value", i->recentlyModifiedRank);
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("coloritemicon");
		elem2->SetAttribute("filename", getName(i->catalogIconColorItemTexResId));
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("colorbgicon");
		elem2->SetAttribute("filename", getName(i->catalogIconColorBGTexResId));
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("greybgicon");
		elem2->SetAttribute("filename", getName(i->catalogIconGreyBGTexResId));
		elem->InsertEndChild(elem2);
		//TODO: binDataOffsetBytes stuff
		//Now insert dependencies for this item
		elem2 = doc->NewElement("depends");
		for(int j = i->firstNormalDepends; j < i->firstNormalDepends + i->numNormalDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("normal");
			elem3->SetAttribute("filename", getName(vNormalDependencies[j].normalTexResId));
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstSoundDepends; j < i->firstSoundDepends + i->numSoundDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("sound");
			elem3->SetAttribute("id", getSoundName(vSoundDependencies[j].soundResId).c_str());
			//elem3->SetAttribute("id", getName(vSoundDependencies[j].soundResId));
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstEffectDepends; j < i->firstEffectDepends + i->numEffectDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("effect");
			elem3->SetAttribute("id", getName(vEffectDependencies[j].effectResId));
			elem2->InsertEndChild(elem3);
		}
		for(int j = i->firstItemDepends; j < i->firstItemDepends + i->numItemDepends; j++)
		{
			XMLElement* elem3 = doc->NewElement("item");
			elem3->SetAttribute("id", mItemNames[vItemDependencies[j].itemResId].c_str());
			elem2->InsertEndChild(elem3);
		}
		elem->InsertEndChild(elem2);
		
		//Now deal with item data
		/*
#define DEFAULT_EXPLODESTREAKS			1
#define DEFAULT_ANIMTHRESHOLD			0
#define DEFAULT_BURNSLOWSANIM			1
#define DEFAULT_CANGETPLAGUE			1
#define DEFAULT_COLLIDEENVIRONMENT		1
#define DEFAULT_COLLIDEITEMS			1
#define DEFAULT_COSTSTAMPS				0
#define DEFAULT_DESCKEY					2
#define DEFAULT_ENABLEFREEZEPOSTANIM	1
#define DEFAULT_ENABLEHFLIP				1
#define DEFAULT_ENABLEFLOORWALLSHADOW	1
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
#define DEFAULT_PLAGUEONCOLLIDE			0*/
		elem2 = doc->NewElement("itemdata");
		//elem2->SetAttribute("itemId", vItemDataHeaders[iCurItemData].itemId);		//Redundant	
		elem2->SetAttribute("itemIdStrId", vItemDataHeaders[iCurItemData].itemIdStrId);
		//elem2->SetAttribute("animResId", vItemDataHeaders[iCurItemData].animResId);	//Redundant
		elem2->SetAttribute("iconAnimBoundsMinx", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.x);
		elem2->SetAttribute("iconAnimBoundsMiny", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.y);
		elem2->SetAttribute("iconAnimBoundsMaxx", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.x);
		elem2->SetAttribute("iconAnimBoundsMaxy", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.y);
		elem2->SetAttribute("nameid", vItemDataHeaders[iCurItemData].name.id);
		elem2->SetAttribute("namekey", vItemDataHeaders[iCurItemData].name.key);
		elem2->SetAttribute("costCoins", vItemDataHeaders[iCurItemData].costCoins);
		elem2->SetAttribute("costStamps", vItemDataHeaders[iCurItemData].costStamps);
		elem2->SetAttribute("valueCoins", vItemDataHeaders[iCurItemData].valueCoins);
		//if(vItemDataHeaders[iCurItemData].valueStamps != DEFAULT_VALUESTAMPS)
			elem2->SetAttribute("valueStamps", vItemDataHeaders[iCurItemData].valueStamps);
		elem2->SetAttribute("descid", vItemDataHeaders[iCurItemData].desc.id);
		elem2->SetAttribute("desckey", vItemDataHeaders[iCurItemData].desc.key);
		//if(vItemDataHeaders[iCurItemData].unlisted != DEFAULT_ULISTED)
			elem2->SetAttribute("unlisted", vItemDataHeaders[iCurItemData].unlisted);
		//if(vItemDataHeaders[iCurItemData].popsCoins != DEFAULT_POPSCOINS)
			elem2->SetAttribute("popsCoins", vItemDataHeaders[iCurItemData].popsCoins);
		elem2->SetAttribute("moneyItem", vItemDataHeaders[iCurItemData].moneyItem);
		elem2->SetAttribute("animThreshold", vItemDataHeaders[iCurItemData].animThreshold);
		elem2->SetAttribute("motorThreshold", vItemDataHeaders[iCurItemData].motorThreshold);
		//if(vItemDataHeaders[iCurItemData].absPosition != DEFAULT_ABS_POSITION)
			elem2->SetAttribute("absPosition", vItemDataHeaders[iCurItemData].absPosition);
		//if(vItemDataHeaders[iCurItemData].scaleVariance != DEFAULT_SCALEVARIANCE)
			elem2->SetAttribute("scaleVariance", vItemDataHeaders[iCurItemData].scaleVariance);
		//if(vItemDataHeaders[iCurItemData].quantity != DEFAULT_QUANTITY)
			elem2->SetAttribute("quantity", vItemDataHeaders[iCurItemData].quantity);
		elem2->SetAttribute("shipTimeSec", vItemDataHeaders[iCurItemData].shipTimeSec);
		elem2->SetAttribute("initialBurnExportId", vItemDataHeaders[iCurItemData].initialBurnExportId);
		elem2->SetAttribute("initialBurnPerGroup", vItemDataHeaders[iCurItemData].initialBurnPerGroup);
		elem2->SetAttribute("mouseGrabbable", vItemDataHeaders[iCurItemData].mouseGrabbable);
		elem2->SetAttribute("illuminate", vItemDataHeaders[iCurItemData].illuminate);
		elem2->SetAttribute("enableHFlip", vItemDataHeaders[iCurItemData].enableHFlip);
		elem2->SetAttribute("floorWallShadow", vItemDataHeaders[iCurItemData].floorWallShadow);
		//if(vItemDataHeaders[iCurItemData].splitJumpLastFrame != DEFAULT_SPLITJUMPLASTFRAME)
			elem2->SetAttribute("splitJumpLastFrame", vItemDataHeaders[iCurItemData].splitJumpLastFrame);
		elem2->SetAttribute("purchaseCooldown", vItemDataHeaders[iCurItemData].purchaseCooldown);
		//if(vItemDataHeaders[iCurItemData].allowDirectionalLight != DEFAULT_DIRLIGHT)
			elem2->SetAttribute("allowDirectionalLight", vItemDataHeaders[iCurItemData].allowDirectionalLight);
		elem2->SetAttribute("mouseGrabSoundResId", vItemDataHeaders[iCurItemData].mouseGrabSoundResId);
		elem2->SetAttribute("instantEffects", vItemDataHeaders[iCurItemData].instantEffects);
		elem2->SetAttribute("freezeOnCollide", vItemDataHeaders[iCurItemData].freezeOnCollide);
		elem2->SetAttribute("enableFreezePostAnim", vItemDataHeaders[iCurItemData].enableFreezePostAnim);
		//if(vItemDataHeaders[iCurItemData].uniqueIgniteSoundResId != DEFAULT_UNIQUEIGNITESOUNDRESID)
			elem2->SetAttribute("uniqueIgniteSoundResId", vItemDataHeaders[iCurItemData].uniqueIgniteSoundResId);
		elem2->SetAttribute("collideItems", vItemDataHeaders[iCurItemData].collideItems);
		elem2->SetAttribute("collideEnvironment", vItemDataHeaders[iCurItemData].collideEnvironment);
		elem2->SetAttribute("orbitalGravity", vItemDataHeaders[iCurItemData].orbitalGravity);
		//if(vItemDataHeaders[iCurItemData].allowExplodeStreaks != DEFAULT_EXPLODESTREAKS)
			elem2->SetAttribute("allowExplodeStreaks", vItemDataHeaders[iCurItemData].allowExplodeStreaks);
		elem2->SetAttribute("burnSlowsAnim", vItemDataHeaders[iCurItemData].burnSlowsAnim);
		elem2->SetAttribute("plagueOnCollide", vItemDataHeaders[iCurItemData].plagueOnCollide);
		//if(vItemDataHeaders[iCurItemData].spawnLimitBurnExportId != DEFAULT_SPAWNLIMITBURNEXPORTID)
			elem2->SetAttribute("spawnLimitBurnExportId", vItemDataHeaders[iCurItemData].spawnLimitBurnExportId);
		elem2->SetAttribute("instAshSoundResId", vItemDataHeaders[iCurItemData].instAshSoundResId);
		elem2->SetAttribute("canGetPlague", vItemDataHeaders[iCurItemData].canGetPlague);
		elem2->SetAttribute("instAshDoesSplat", vItemDataHeaders[iCurItemData].instAshDoesSplat);
		elem2->SetAttribute("modXAmpMin", vItemDataHeaders[iCurItemData].modXAmpMin);
		elem2->SetAttribute("modXAmpMax", vItemDataHeaders[iCurItemData].modXAmpMax);
		elem2->SetAttribute("modXFreqMin", vItemDataHeaders[iCurItemData].modXFreqMin);
		elem2->SetAttribute("modXFreqMax", vItemDataHeaders[iCurItemData].modXFreqMax);
		elem2->SetAttribute("modXPhaseMin", vItemDataHeaders[iCurItemData].modXPhaseMin);
		elem2->SetAttribute("modXPhaseMax", vItemDataHeaders[iCurItemData].modXPhaseMax);
		elem2->SetAttribute("modXSpeedMin", vItemDataHeaders[iCurItemData].modXSpeedMin);
		elem2->SetAttribute("modXSpeedMax", vItemDataHeaders[iCurItemData].modXSpeedMax);
		elem2->SetAttribute("modYAmpMin", vItemDataHeaders[iCurItemData].modYAmpMin);
		elem2->SetAttribute("modYAmpMax", vItemDataHeaders[iCurItemData].modYAmpMax);
		elem2->SetAttribute("modYFreqMin", vItemDataHeaders[iCurItemData].modYFreqMin);
		elem2->SetAttribute("modYFreqMax", vItemDataHeaders[iCurItemData].modYFreqMax);
		elem2->SetAttribute("modYPhaseMin", vItemDataHeaders[iCurItemData].modYPhaseMin);
		elem2->SetAttribute("modYPhaseMax", vItemDataHeaders[iCurItemData].modYPhaseMax);
		elem2->SetAttribute("modYSpeedMin", vItemDataHeaders[iCurItemData].modYSpeedMin);
		elem2->SetAttribute("modYSpeedMax", vItemDataHeaders[iCurItemData].modYSpeedMax);
		elem->InsertEndChild(elem2);
		
		//Now loop back through here and pull data from it all
		for(const XMLAttribute* att = elem2->FirstAttribute(); att != NULL; att = att->Next())
			mOccurrences[att->Name()].push_back(att->Value());
		
		
		//XMLElement* elem3 = doc->NewElement("itemId");
		//elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].itemId);
		//elem2->InsertEndChild(elem3);
		/*elem2 = doc->NewElement("itemdata");
		XMLElement* elem3 = doc->NewElement("itemIdStrId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].itemIdStrId);
		elem2->InsertEndChild(elem3);
		//elem3 = doc->NewElement("animResId");
		//elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].animResId);
		//elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("iconAnimBoundsMin");
		elem3->SetAttribute("x", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.x);
		elem3->SetAttribute("y", vItemDataHeaders[iCurItemData].iconAnimBoundsMin.y);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("iconAnimBoundsMax");
		elem3->SetAttribute("x", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.x);
		elem3->SetAttribute("y", vItemDataHeaders[iCurItemData].iconAnimBoundsMax.y);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("name");
		elem3->SetAttribute("id", vItemDataHeaders[iCurItemData].name.id);
		elem3->SetAttribute("key", vItemDataHeaders[iCurItemData].name.key);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("costCoins");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].costCoins);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("costStamps");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].costStamps);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("valueCoins");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].valueCoins);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("valueStamps");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].valueStamps);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("desc");
		elem3->SetAttribute("id", vItemDataHeaders[iCurItemData].desc.id);
		elem3->SetAttribute("key", vItemDataHeaders[iCurItemData].desc.key);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("unlisted");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].unlisted);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("popsCoins");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].popsCoins);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("moneyItem");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].moneyItem);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("animThreshold");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].animThreshold);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("motorThreshold");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].motorThreshold);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("absPosition");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].absPosition);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("scaleVariance");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].scaleVariance);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("quantity");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].quantity);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("shipTimeSec");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].shipTimeSec);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("initialBurnExportId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].initialBurnExportId);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("initialBurnPerGroup");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].initialBurnPerGroup);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("mouseGrabbable");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].mouseGrabbable);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("illuminate");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].illuminate);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("enableHFlip");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].enableHFlip);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("floorWallShadow");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].floorWallShadow);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("splitJumpLastFrame");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].splitJumpLastFrame);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("purchaseCooldown");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].purchaseCooldown);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("allowDirectionalLight");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].allowDirectionalLight);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("mouseGrabSoundResId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].mouseGrabSoundResId);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("instantEffects");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].instantEffects);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("freezeOnCollide");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].freezeOnCollide);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("enableFreezePostAnim");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].enableFreezePostAnim);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("uniqueIgniteSoundResId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].uniqueIgniteSoundResId);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("collideItems");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].collideItems);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("collideEnvironment");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].collideEnvironment);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("orbitalGravity");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].orbitalGravity);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("allowExplodeStreaks");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].allowExplodeStreaks);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("burnSlowsAnim");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].burnSlowsAnim);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("plagueOnCollide");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].plagueOnCollide);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("spawnLimitBurnExportId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].spawnLimitBurnExportId);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("instAshSoundResId");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].instAshSoundResId);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("canGetPlague");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].canGetPlague);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("instAshDoesSplat");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].instAshDoesSplat);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXAmpMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXAmpMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXAmpMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXAmpMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXFreqMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXFreqMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXFreqMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXFreqMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXPhaseMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXPhaseMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXPhaseMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXPhaseMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXSpeedMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXSpeedMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modXSpeedMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modXSpeedMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYAmpMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYAmpMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYAmpMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYAmpMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYFreqMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYFreqMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYFreqMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYFreqMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYPhaseMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYPhaseMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYPhaseMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYPhaseMax);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYSpeedMin");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYSpeedMin);
		elem2->InsertEndChild(elem3);
		elem3 = doc->NewElement("modYSpeedMax");
		elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].modYSpeedMax);
		elem2->InsertEndChild(elem3);*/
		//elem3 = doc->NewElement("");
		//elem3->SetAttribute("value", vItemDataHeaders[iCurItemData].);
		//elem2->InsertEndChild(elem3);
		iCurItemData++;
		//TODO: Write rest of XML stuff for rest of item data
		elem->InsertEndChild(elem2);
		root->InsertEndChild(elem);
	}
	
	doc->InsertFirstChild(root);
	doc->SaveFile(sFilename.c_str());
	
	delete doc;
	
	//DEBUG: Save out all the data that we got in our map
	ofstream ofile("map2.txt");
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
			ofile << i->first << " had the default value " << sHighest << ", which occurred " << iHighest << " times." << endl;
		//ofile << *j << " ";
		//ofile << endl << endl;
	}
	//
	
	return true;
}

bool XMLToItemManifest(const char* cFilename)
{
	//Open this XML file for parsing
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
			cout << "Error: Unable to get id of XML element in file " << sXMLFile << endl;
			delete doc;
			return false;
		}
		//imr.itemId = HASH(id); //TODO
		imr.recentlyModifiedRank = 1;	//Because why not
		
		//get all the XML children of this
		for(XMLElement* elem2 = elem->FirstChildElement(); elem2 != NULL; elem2 = elem2->NextSiblingElement())
		{
			string sName = elem2->Name();
			if(sName == "animresid")
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for animresid in file " << sXMLFile << endl;
					delete doc;
					return false;
				}
				imr.animResId = getResID(cTemp);
			}
			else if(sName == "recentlymodifiedrank")
			{
				//Completely ignore //TODO Remove test case
			}
			else if(sName == "coloritemicon")
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for coloritemicon in file " << sXMLFile << endl;
					delete doc;
					return false;
				}
				imr.catalogIconColorItemTexResId = getResID(cTemp);
			}
			else if(sName == "colorbgicon")
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for colorbgicon in file " << sXMLFile << endl;
					delete doc;
					return false;
				}
				imr.catalogIconColorBGTexResId = getResID(cTemp);
			}
			else if(sName == "greybgicon")
			{
				const char* cTemp = elem2->Attribute("filename");
				if(cTemp == NULL)
				{
					cout << "Error: Missing filename for greybgicon in file " << sXMLFile << endl;
					delete doc;
					return false;
				}
				imr.catalogIconGreyBGTexResId = getResID(cTemp);
			}
			//TODO: binDataOffsetBytes
			else if(sName == "depends")
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
					string sDependName = elem3->Name();
					if(sDependName == "normal")
					{
						const char* cTemp = elem3->Attribute("filename");
						if(cTemp == NULL)
						{
							cout << "Error: Missing filename for normal dependency in file " << sXMLFile << endl;
							delete doc;
							return false;
						}
						normalDependency nd;
						nd.normalTexResId = getResID(cTemp);
						lNormalDeps.push_back(nd);
						imr.numNormalDepends++;
					}
					else if(sDependName == "sound")
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for sound dependency in file " << sXMLFile << endl;
							delete doc;
							return false;
						}
						soundDependency sd;
						sd.soundResId = getSoundId(cTemp);
						lSoundDeps.push_back(sd);
						imr.numSoundDepends++;
					}
					else if(sDependName == "effect")
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for effect dependency in file " << sXMLFile << endl;
							delete doc;
							return false;
						}
						effectDependency ed;
						ed.effectResId = getResID(cTemp);
						lEffectDeps.push_back(ed);
						imr.numEffectDepends++;
					}
					else if(sDependName == "item")
					{
						const char* cTemp = elem3->Attribute("id");
						if(cTemp == NULL)
						{
							cout << "Error: Missing id for item dependency in file " << sXMLFile << endl;
							delete doc;
							return false;
						}
						itemDependency id;
						//TODO id.itemResId = HASH(cTemp);
						lItemDeps.push_back(id);
						imr.numItemDepends++;
					}
					else if(sDependName == "")
						cout << "Warning: Empty element name for depends in XML file " << sXMLFile << endl;
					else
						cout << "Warning: Unknown name for dependency: " << sDependName << " in XML file " << sXMLFile << ". Ignoring..." << endl;
				}
			}
			else if(sName == "")
				cout << "Warning: XML element missing name in file " << sXMLFile << ". Ignoring... " << endl;
			else
				cout << "Warning: Unknown XML element name: " << sName << " in XML file " << sXMLFile << ". Ignoring..." << endl;
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
	string sFilename = cFilename;
	sFilename += ".derp";	//TODO: Replace original file
	FILE* f = fopen(sFilename.c_str(), "wb");
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
	
	fclose(f);
	return true;
}


















