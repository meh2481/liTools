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
	fclose(f);
	
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


















