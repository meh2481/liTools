#include "pakDataTypes.h"

bool LoctexManifestToXML(wstring sFilename)
{
	FILE* f = _wfopen(sFilename.c_str(), TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(sFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in our header
	loctexManifestHeader lmh;
	if(fread(&lmh, 1, sizeof(loctexManifestHeader), f) != sizeof(loctexManifestHeader))
	{
		cout << "Error: unable to read loctexManifestHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in loctexFiles
	fseek(f, lmh.files.offset, SEEK_SET);
	list<loctexFile> lFiles;
	for(int i = 0; i < lmh.files.count; i++)
	{
		loctexFile lf;
		if(fread(&lf, 1, sizeof(loctexFile), f) != sizeof(loctexFile))
		{
			cout << "Error: unable to read loctexFile " << i << " from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lFiles.push_back(lf);
	}
	
	//Read in loctexRecords
	fseek(f, lmh.records.offset, SEEK_SET);
	vector<loctexRecord> vRecords;
	for(int i = 0; i < lmh.records.count; i++)
	{
		loctexRecord lr;
		if(fread(&lr, 1, sizeof(loctexRecord), f) != sizeof(loctexRecord))
		{
			cout << "Error: unable to read loctexRecord " << i << " from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		vRecords.push_back(lr);
	}
	
	//Done here
	fclose(f);
	
	//Now, open up the XML file and put all the data into it
	sFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("loctexmanifest");	//Create the root element
	
	//Write out files
	for(list<loctexFile>::iterator i = lFiles.begin(); i != lFiles.end(); i++)
	{
		XMLElement* elem = doc->NewElement("file");
		elem->SetAttribute("filename", ws2s(getName(i->resId)).c_str());
		//Write out records for this file
		for(int j = i->index; j < i->index + i->count; j++)
		{
			XMLElement* elem2 = doc->NewElement("record");
			//elem2->SetAttribute("langid", vRecords[j].languageId);
			elem2->SetAttribute("lang", ws2s(toLangString(vRecords[j].languageId)).c_str());
			elem2->SetAttribute("langfilename", ws2s(getName(vRecords[j].localizedResId)).c_str());
			elem->InsertEndChild(elem2);
		}
		root->InsertEndChild(elem);
	}
	
	doc->InsertFirstChild(root);
	doc->SaveFile(ws2s(sFilename).c_str());
	
	return true;
}

bool XMLToLoctexManifest(wstring sFilename)
{
	//Open this XML file for parsing
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
		cout << "Error: No root element in XML file " << ws2s(sXMLFile) << endl;
		delete doc;
		return false;
	}
	
	//Start parsing in files
	list<loctexFile> lFiles;
	list<loctexRecord> lRecords;
	for(XMLElement* elem = root->FirstChildElement("file"); elem != NULL; elem = elem->NextSiblingElement("file"))
	{
		const char* id = elem->Attribute("filename");
		if(id == NULL) continue;
		loctexFile lf;
		lf.resId = getResID(s2ws(id));
		lf.index = lRecords.size();
		lf.count = 0;
		//Get records for this file
		for(XMLElement* elem2 = elem->FirstChildElement("record"); elem2 != NULL; elem2 = elem2->NextSiblingElement("record"))
		{
			const char* resid = elem2->Attribute("langfilename");
			if(resid == NULL) continue;
			loctexRecord lr;
			lr.languageId = LANGID_ENGLISH;
			const char* lang = elem2->Attribute("lang");
			if(lang == NULL) continue;
			lr.languageId = toLangID(s2ws(lang));
			//if(elem2->QueryUnsignedAttribute("langid", &lr.languageId) != XML_NO_ERROR) continue;
			lr.localizedResId = getResID(s2ws(resid));
			lRecords.push_back(lr);
			lf.count++;
		}
		if(lf.count)
			lFiles.push_back(lf);
	}
	
	delete doc;	//Done with this
	
	//Open our output file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(sFilename) << endl;
		return false;
	}
	
	//Write header
	loctexManifestHeader lmh;
	lmh.files.count = lFiles.size();
	lmh.files.offset = sizeof(loctexManifestHeader);
	lmh.records.count = lRecords.size();
	lmh.records.offset = sizeof(loctexManifestHeader) + lFiles.size() * sizeof(loctexFile);
	
	fwrite(&lmh, 1, sizeof(loctexManifestHeader), f);
	
	//Write out files
	for(list<loctexFile>::iterator i = lFiles.begin(); i != lFiles.end(); i++)
		fwrite(&(*i), 1, sizeof(loctexFile), f);
		
	//Write out records
	for(list<loctexRecord>::iterator i = lRecords.begin(); i != lRecords.end(); i++)
		fwrite(&(*i), 1, sizeof(loctexRecord), f);
		
	fclose(f);
	
	return true;
}
















