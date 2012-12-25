#include "pakDataTypes.h"
#include <tinyxml2.h>
#include <cstring>
#include <string>
#include <list>
#include <map>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
using namespace tinyxml2;

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
	
	//Finally, read in the itemDependencies
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
	
	//Now, open up the XML file and put all the data into it
	string sFilename = cFilename;
	sFilename += ".xml";
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("itemmanifest");	//Create the root element
	
	for(list<itemManifestRecord>::iterator i = lManifestRecords.begin(); i != lManifestRecords.end(); i++)
	{
		XMLElement* elem = doc->NewElement("itemrecord");
		elem->SetAttribute("id", mItemNames[i->itemId].c_str());
		XMLElement* elem2 = doc->NewElement("animresid");
		elem2->SetAttribute("filename", getName(i->animResId));
		elem->InsertEndChild(elem2);
		elem2 = doc->NewElement("recentlymodifiedrank");
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
		
		root->InsertEndChild(elem);
	}
	
	//TODO: Rest of file spec
	
	doc->InsertFirstChild(root);
	doc->SaveFile(sFilename.c_str());
	
	delete doc;
	
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
	
	//TODO
	
	
	delete doc;
	return true;
}


















