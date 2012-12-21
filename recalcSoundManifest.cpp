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

const char* cFilename = "vdata/sndmanifest.dat.xml";

int main(int argc, char** argv)
{
	DWORD iTicks = GetTickCount();
	
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(cFilename);
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << cFilename << ": Error " << iErr << endl;
		delete doc;
		return false;
	}
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << cFilename << endl;
		delete doc;
		return false;
	}
	int iTotalSounds = 0;
	root->QueryIntAttribute("numtakes", &iTotalSounds);	
	
	//Roll through child elements
	XMLElement* elem = root->FirstChildElement("sound");
	int iCurSound = 0;
	while(elem != NULL)
	{
		//Get the takes for this sound
		XMLElement* elem2 = elem->FirstChildElement("take");
		while(elem2 != NULL)
		{
			const char* cName = elem2->Attribute("filename");
			if(cName == NULL)
			{
				cout << "Error: Unable to get filename of take record in file " << cFilename << endl;
				delete doc;
				return false;
			}
			cout << "Parsing sound " << ++iCurSound << " out of " << iTotalSounds << ": " << cName << endl;
			takeRecord tr = getOggData(cName);	//Grab the header data from the sound file.
			//Update this info
			elem2->SetAttribute("channels", tr.channels);
			elem2->SetAttribute("samplespersec", tr.samplesPerSec);
			elem2->SetAttribute("samplecountperchannel", tr.sampleCountPerChannel);
			elem2->SetAttribute("vorbisworkingsetsizebytes", tr.vorbisWorkingSetSizeBytes);
			elem2->SetAttribute("vorbismarkerssizebytes", tr.vorbisMarkersSizeBytes);
			elem2->SetAttribute("vorbispacketssizebytes", tr.vorbisPacketsSizeBytes);
			elem2 = elem2->NextSiblingElement("take");	//Next item
		}
		elem = elem->NextSiblingElement("sound");	//Next item
	}
	
	doc->SaveFile(cFilename);	//Save this back
	delete doc;	//We're done with this
	
	iTicks = GetTickCount() - iTicks;
	int iSeconds = iTicks / 1000;	//Get seconds elapsed
	int iMinutes = iSeconds / 60;
	iSeconds -= iMinutes * 60;
	
	cout << "Time elapsed: " << iMinutes << " min, " << iSeconds << " sec" << endl;
	return 0;
}





