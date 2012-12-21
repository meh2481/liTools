#include <tinyxml2.h>
#include "pakDataTypes.h"
#include <cstring>
#include <string>
#include <list>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
using namespace tinyxml2;

bool sndManifestToXML(const char* cFilename)
{
	FILE* f = fopen(cFilename, "rb");
	if(f == NULL)
	{
		cout << "Unable to open " << cFilename << endl;
		return false;
	}
	
	//Read in the soundManifestHeader
	soundManifestHeader smh;
	if(fread(&smh, 1, sizeof(soundManifestHeader), f) != sizeof(soundManifestHeader))
	{
		cout << "Unable to read soundManifestHeader in file " << cFilename << endl;
		fclose(f);
		return false;
	}
	
	//Read in the take groups
	fseek(f, smh.sounds.offset, SEEK_SET);
	list<soundTakeGroup> lSoundTakeGroups;
	for(i32 i = 0; i < smh.sounds.count; i++)
	{
		soundTakeGroup stg;
		if(fread(&stg, 1, sizeof(soundTakeGroup), f) != sizeof(soundTakeGroup))
		{
			cout << "Error reading sound take group " << i << " in file " << cFilename << endl;
			return false;
		}
		lSoundTakeGroups.push_back(stg);
	}
	
	//Read in the takes
	fseek(f, smh.takes.offset, SEEK_SET);
	vector<takeRecord> vSoundTakes;
	for(i32 i = 0; i < smh.takes.count; i++)
	{
		takeRecord tr;
		if(fread(&tr, 1, sizeof(takeRecord), f) != sizeof(takeRecord))
		{
			cout << "Error reading take record " << i << " in file " << cFilename << endl;
			return false;
		}
		vSoundTakes.push_back(tr);
	}
	
	//Done reading the file. Now parse it out to XML
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("soundmanifest");	//Create the root element
	root->SetAttribute("numsounds", lSoundTakeGroups.size());
	root->SetAttribute("numtakes", vSoundTakes.size());
	
	for(list<soundTakeGroup>::iterator i = lSoundTakeGroups.begin(); i != lSoundTakeGroups.end(); i++)
	{
		XMLElement* elem = doc->NewElement("sound");
		elem->SetAttribute("id", i->logicalId);
		//Now insert takes for this sound
		for(int j = i->firstTakeIdx; j < i->firstTakeIdx + i->numTakes; j++)
		{
			XMLElement* elem2 = doc->NewElement("take");
			string sFilename = getName(vSoundTakes[j].resId);
			sFilename += ".ogg";
			elem2->SetAttribute("filename", sFilename.c_str());
			elem2->SetAttribute("channels", vSoundTakes[j].channels);
			elem2->SetAttribute("samplespersec", vSoundTakes[j].samplesPerSec);
			elem2->SetAttribute("samplecountperchannel", vSoundTakes[j].sampleCountPerChannel);
			elem2->SetAttribute("vorbisworkingsetsizebytes", vSoundTakes[j].vorbisWorkingSetSizeBytes);
			elem2->SetAttribute("vorbismarkerssizebytes", vSoundTakes[j].vorbisMarkersSizeBytes);
			elem2->SetAttribute("vorbispacketssizebytes", vSoundTakes[j].vorbisPacketsSizeBytes);
			//Add this element
			elem->InsertEndChild(elem2);
		}
		root->InsertEndChild(elem);
	}
	
	doc->InsertFirstChild(root);
	string sFilename = cFilename;
	sFilename += ".xml";
	doc->SaveFile(sFilename.c_str());
	
	delete doc;
	
	return true;
}

bool XMLToSndManifest(const char* cFilename)
{
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
	
	//Roll through child elements
	list<soundTakeGroup> lSoundTakeGroups;
	vector<takeRecord> vSoundTakes;
	XMLElement* elem = root->FirstChildElement("sound");
	i32 iCurTake = 0;
	while(elem != NULL)
	{
		soundTakeGroup stg;
		if(elem->QueryUnsignedAttribute("id", &stg.logicalId) != XML_NO_ERROR)
		{
			cout << "Error: Unable to get id of XML element in file " << sXMLFile << endl;
			delete doc;
			return false;
		}
		stg.firstTakeIdx = vSoundTakes.size();
		stg.numTakes = 0;
		
		//Get the takes for this sound
		XMLElement* elem2 = elem->FirstChildElement("take");
		while(elem2 != NULL)
		{
			iCurTake++;
			takeRecord tr;
			const char* cName = elem2->Attribute("filename");
			if(cName == NULL)
			{
				cout << "Error: Unable to get filename of take record in file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("channels", &tr.channels) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get channels from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("samplespersec", &tr.samplesPerSec) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get samplesPerSec from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("samplecountperchannel", &tr.sampleCountPerChannel) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get sampleCountPerChannel from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("vorbisworkingsetsizebytes", &tr.vorbisWorkingSetSizeBytes) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get vorbisWorkingSetSizeBytes from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("vorbismarkerssizebytes", &tr.vorbisMarkersSizeBytes) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get vorbisMarkersSizeBytes from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			if(elem2->QueryIntAttribute("vorbispacketssizebytes", &tr.vorbisPacketsSizeBytes) != XML_NO_ERROR)
			{
				cout << "Error: Unable to get vorbisPacketsSizeBytes from take " << iCurTake << " from file " << sXMLFile << endl;
				delete doc;
				return false;
			}
			string sName = cName;
			sName.erase(sName.size()-4);	//Delete the ".ogg" ending from the string
			tr.resId = getResID(sName);		//Get the resource ID from this filename
			vSoundTakes.push_back(tr);
			elem2 = elem2->NextSiblingElement("take");
			stg.numTakes++;
		}
		
		lSoundTakeGroups.push_back(stg);	//Hang onto this
		elem = elem->NextSiblingElement("sound");	//Next item
	}
	delete doc;	//We're done with this
	
	//Repack
	FILE* f = fopen(cFilename, "wb");	//Open file for writing
	if(f == NULL)
	{
		cout << "Unable to open file " << cFilename << " for writing." << endl;
		return false;
	}
	
	//Write the soundManifestHeader
	i32 curOffset = sizeof(soundManifestHeader);
	soundManifestHeader smh;
	smh.sounds.count = lSoundTakeGroups.size();
	smh.sounds.offset = curOffset;
	smh.takes.count = vSoundTakes.size();
	curOffset += lSoundTakeGroups.size() * sizeof(soundTakeGroup);
	smh.takes.offset = curOffset;
	fwrite(&smh, 1, sizeof(soundManifestHeader), f);
	
	//Write out sounds
	for(list<soundTakeGroup>:: iterator i = lSoundTakeGroups.begin(); i != lSoundTakeGroups.end(); i++)
		fwrite(&(*i), 1, sizeof(soundTakeGroup), f);
	
	//Write out sound takes
	for(unsigned int i = 0; i < vSoundTakes.size(); i++)
		fwrite(&vSoundTakes[i], 1, sizeof(takeRecord), f);
	
	//Done	
	fclose(f);
	return true;
}








