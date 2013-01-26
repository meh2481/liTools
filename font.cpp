#include "pakDataTypes.h"

bool fontManifestToXML(wstring sFilename)
{
	FILE* f = _wfopen(sFilename.c_str(), TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(sFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in the header
	fontManifestHeader fmh;
	if(fread(&fmh, 1, sizeof(fontManifestHeader), f) != sizeof(fontManifestHeader))
	{
		cout << "Error: unable to read fontManifestHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in fontManifestRecords
	list<fontManifestRecord> lFonts;
	for(int i = 0; i < fmh.numFonts; i++)
	{
		fontManifestRecord fmr;
		if(fread(&fmr, 1, sizeof(fontManifestRecord), f) != sizeof(fontManifestRecord))
		{
			cout << "Error: unable to read fontManifestRecord from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lFonts.push_back(fmr);
	}
	
	//Read in the texture list until we hit EOF, because apparently we don't have a BinHdrPtr for this...
	vector<fontManifestTexture> vTextures;
	for(;;)
	{
		fontManifestTexture fmt;
		if(fread(&fmt, 1, sizeof(fontManifestTexture), f) != sizeof(fontManifestTexture))
			break; //Done
		vTextures.push_back(fmt);
	}
	
	//Ok, done
	fclose(f);
	
	//Now output this to XML
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("fontmanifest");
	
	for(list<fontManifestRecord>::iterator i = lFonts.begin(); i != lFonts.end(); i++)
	{
		XMLElement* elem = doc->NewElement("font");
		elem->SetAttribute("filename", ws2s(getName(i->fontResId)).c_str());
		for(int j = i->firstTexDependsIdx; j < i->firstTexDependsIdx + i->numTexDepends; j++)
		{
			XMLElement* elem2 = doc->NewElement("texture");
			elem2->SetAttribute("filename", ws2s(getName(vTextures[j].texResId)).c_str());
			elem->InsertEndChild(elem2);
		}
		root->InsertEndChild(elem);
	}
	
	doc->InsertFirstChild(root);
	wstring sXMLFilename = sFilename;
	sXMLFilename += TEXT(".xml");
	doc->SaveFile(ws2s(sXMLFilename).c_str());
	
	delete doc;
	return true;
}

bool XMLToFontManifest(wstring sFilename)
{
	wstring sXMLFilename = sFilename;
	sXMLFilename += TEXT(".xml");
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(ws2s(sXMLFilename).c_str());
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << ws2s(sXMLFilename) << ": Error " << iErr << endl;
		delete doc;
		return false;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << ws2s(sXMLFilename) << endl;
		delete doc;
		return false;
	}
	
	//Read in fonts
	list<fontManifestRecord> lFonts;
	list<fontManifestTexture> lFontTextures;
	for(XMLElement* font = root->FirstChildElement("font"); font != NULL; font = font->NextSiblingElement("font"))
	{
		const char* cIDFilename = font->Attribute("filename");
		if(cIDFilename == NULL) continue;	//Skip over broken filename
		fontManifestRecord fmr;
		fmr.fontResId = getResID(s2ws(cIDFilename));
		fmr.firstTexDependsIdx = lFontTextures.size();
		fmr.numTexDepends = 0;
		for(XMLElement* texture = font->FirstChildElement("texture"); texture != NULL; texture = texture->NextSiblingElement("texture"))
		{
			const char* cTexFilename = texture->Attribute("filename");
			if(cTexFilename == NULL) continue;
			fontManifestTexture fmt;
			fmt.texResId = getResID(s2ws(cTexFilename));
			lFontTextures.push_back(fmt);
			fmr.numTexDepends++;
		}
		lFonts.push_back(fmr);
	}
	
	delete doc;	//Done with this
	
	//Open our output file
	FILE* f = _wfopen(sFilename.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(sFilename) << endl;
		return false;
	}
	
	//Write out our fontManifestHeader
	fontManifestHeader fmh;
	fmh.numFonts = lFonts.size();
	fwrite(&fmh, 1, sizeof(fontManifestHeader), f);
	
	//Write out our fonts
	for(list<fontManifestRecord>::iterator i = lFonts.begin(); i != lFonts.end(); i++)
		fwrite(&(*i), 1, sizeof(fontManifestRecord), f);
	
	//Write out our font textures
	for(list<fontManifestTexture>::iterator i = lFontTextures.begin(); i != lFontTextures.end(); i++)
		fwrite(&(*i), 1, sizeof(fontManifestTexture), f);
	
	//Done
	fclose(f);
	return true;
}

string getFontChar(i32 codepoint)
{
	//codepoint is a raw Unicode number which we'll convert to UTF-8 for TinyXML
	string sReturn;
	if(codepoint <= 0x7F)	//One byte
	{
		char c = codepoint;
		sReturn.push_back(c);
		sReturn.push_back('\0');
	}
	else if(codepoint <= 0x7FF)	//Two bytes
	{
		char c = 0xC0;	//First byte
		c |= codepoint >> 6;
		sReturn.push_back(c);
		c = 0x80;		//Second byte
		c |= codepoint & 0x3F;
		sReturn.push_back(c);
		sReturn.push_back('\0');
	}
	else if(codepoint <= 0xFFFF) //Three bytes
	{
		char c = 0xE0;	//First byte
		c |= codepoint >> 12;
		sReturn.push_back(c);
		c = 0x80;		//Second byte
		c |= (codepoint >> 6) & 0x3F;
		sReturn.push_back(c);
		c = 0x80;		//Third byte
		c |= codepoint & 0x3F;
		sReturn.push_back(c);
		sReturn.push_back('\0');
	}
	else	//We don't support anything higher, and the game doesn't either (at least for kerning)
	{
		cout << "Unsupported code point for UTF data: " << codepoint << endl;
		return sReturn;
	}
	
	return sReturn;
}

bool fontToXML(wstring sFilename)
{
	FILE* f = _wfopen(sFilename.c_str(), TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(sFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in the header
	fontResourceHeader frh;
	if(fread(&frh, 1, sizeof(fontResourceHeader), f) != sizeof(fontResourceHeader))
	{
		cout << "Error: unable to read fontResourceHeader from file " << ws2s(sFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in fontCharacterRecords
	fseek(f, frh.chars.offset, SEEK_SET);
	list<fontCharacterRecord> lFontChars;
	for(int i = 0; i < frh.chars.count; i++)
	{
		fontCharacterRecord fcr;
		if(fread(&fcr, 1, sizeof(fontCharacterRecord), f) != sizeof(fontCharacterRecord))
		{
			cout << "Error: unable to read fontCharacterRecord from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lFontChars.push_back(fcr);
	}
	
	//Read in fontKerningRecords
	fseek(f, frh.kerns.offset, SEEK_SET);
	list<fontKerningRecord> lFontKerns;
	for(int i = 0; i < frh.kerns.count; i++)
	{
		fontKerningRecord fkr;
		if(fread(&fkr, 1, sizeof(fontKerningRecord), f) != sizeof(fontKerningRecord))
		{
			cout << "Error: unable to read fontKerningRecord from file " << ws2s(sFilename) << endl;
			fclose(f);
			return false;
		}
		lFontKerns.push_back(fkr);
	}
	
	//Ok, done
	fclose(f);
	
	//Now output this to XML
	XMLDocument* doc = new XMLDocument;
	//doc->SetBOM(true);
	//doc->Parse("<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>");
	XMLElement* root = doc->NewElement("font");
	root->SetAttribute("pointsize", frh.pointSize);
	root->SetAttribute("extleading", frh.extLeading);
	root->SetAttribute("maxascent", frh.maxAscent);
	root->SetAttribute("maxdescent", frh.maxDescent);
	
	//Write out our font characters
	for(list<fontCharacterRecord>::iterator i = lFontChars.begin(); i != lFontChars.end(); i++)
	{
		XMLElement* elem = doc->NewElement("char");
		elem->SetAttribute("value", getFontChar(i->codepoint).c_str());
		elem->SetAttribute("texpage", i->texPageIdx);
		elem->SetAttribute("texx", i->texX);
		elem->SetAttribute("texy", i->texY);
		elem->SetAttribute("texw", i->texW);
		elem->SetAttribute("texh", i->texH);
		elem->SetAttribute("offsetx", i->offsetX);
		elem->SetAttribute("offsety", i->offsetY);
		elem->SetAttribute("advance", i->advance);
		root->InsertEndChild(elem);
	}
	
	//Write out our font kernings
	for(list<fontKerningRecord>::iterator i = lFontKerns.begin(); i != lFontKerns.end(); i++)
	{
		XMLElement* elem = doc->NewElement("kerning");
		elem->SetAttribute("char1", getFontChar(i->codepoints[0]).c_str());
		elem->SetAttribute("char2", getFontChar(i->codepoints[1]).c_str());
		elem->SetAttribute("amount", i->kernAmount);
		root->InsertEndChild(elem);
	}
	
	doc->InsertEndChild(root);
	wstring sXMLFilename = sFilename;
	//sXMLFilename += TEXT(".temp.xml"); //TODO Overwrite old file
	doc->SaveFile(ws2s(sXMLFilename).c_str());
	
	delete doc;
	return true;
}

//ofstream utffile("utftest.txt");
i32 getFontCodepoint(const char* cText)
{
	i32 codepoint = 0;
	if(cText == NULL || cText[0] == '\0')
	{
		return codepoint;
	}
	unsigned char c = cText[0];
	if(c >> 7 == 0)	//Case 1: One byte
	{
		codepoint = c;
		//utffile << "Case 1: codepoint " << codepoint << " strlen: " << strlen(cText) << endl;
	}
	else if(c >> 5 == 0x6)	//Case 2: Two bytes
	{
		codepoint = (c & 0x1F) << 6;
		c = cText[1];
		codepoint |= c & 0x3F;
		//utffile << "Case 2: codepoint " << codepoint << endl;
	}
	else if(c >> 4 == 0xE)	//Case 3: Three bytes
	{
		codepoint = (c & 0xF) << 12;
		c = cText[1];
		codepoint |= (c & 0x3F) << 6;
		c = cText[2];
		codepoint |= c & 0x3F;
		//utffile << "Case 3: codepoint " << codepoint << endl;
	}
	
	return codepoint;
}

bool XMLToFont(wstring sFilename)
{
	XMLDocument* doc = new XMLDocument;
	//doc->SetBOM(true);
	int iErr = doc->LoadFile(ws2s(sFilename).c_str());
	if(iErr != XML_NO_ERROR)
	{
		cout << "Error parsing XML file " << ws2s(sFilename) << ": Error " << iErr << endl;
		delete doc;
		return false;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		cout << "Error: Root element NULL in XML file " << ws2s(sFilename) << endl;
		delete doc;
		return false;
	}
	
	//Read in fontResourceHeader stuff
	fontResourceHeader frh;
	root->QueryIntAttribute("pointsize", &frh.pointSize);
	root->QueryFloatAttribute("extleading", &frh.extLeading);
	root->QueryFloatAttribute("maxascent", &frh.maxAscent);
	root->QueryFloatAttribute("maxdescent", &frh.maxDescent);
	
	//Read in XML character and kerning records
	list<fontCharacterRecord> lCharRecords;
	list<fontKerningRecord> lKerningRecords;
	for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		const char* cName = elem->Name();
		if(cName == NULL) continue;
		string sName = cName;
		if(sName == "char")	//Character
		{
			fontCharacterRecord fcr;
			fcr.texPageIdx = 0;	//Set default values because we're using QueryXAttribute() stuff
			fcr.offsetX = 0.0;
			fcr.offsetY = 0.0;
			fcr.advance = 0.0;
			
			fcr.codepoint = getFontCodepoint(elem->Attribute("value"));
			elem->QueryIntAttribute("texpage", &fcr.texPageIdx);
			int iTemp = 0;
			elem->QueryIntAttribute("texx", &iTemp);
			fcr.texX = iTemp;
			iTemp = 0;
			elem->QueryIntAttribute("texy", &iTemp);
			fcr.texY = iTemp;
			iTemp = 0;
			elem->QueryIntAttribute("texw", &iTemp);
			fcr.texW = iTemp;
			iTemp = 0;
			elem->QueryIntAttribute("texh", &iTemp);
			fcr.texH = iTemp;
			iTemp = 0;
			elem->QueryFloatAttribute("offsetx", &fcr.offsetX);
			elem->QueryFloatAttribute("offsety", &fcr.offsetY);
			elem->QueryFloatAttribute("advance", &fcr.advance);
			
			lCharRecords.push_back(fcr);
		}
		else if(sName == "kerning")	//Kerning value
		{
			fontKerningRecord fkr;
			fkr.kernAmount = 0.0;
			
			fkr.codepoints[0] = getFontCodepoint(elem->Attribute("char1"));
			fkr.codepoints[1] = getFontCodepoint(elem->Attribute("char2"));
			elem->QueryFloatAttribute("amount", &fkr.kernAmount);
			
			lKerningRecords.push_back(fkr);
		}
	}
	
	delete doc;	//Done parsing
	
	//Open our output file
	wstring sOut = sFilename;
	sOut += TEXT(".temp");
	FILE* f = _wfopen(sOut.c_str(), TEXT("wb"));
	if(f == NULL)
	{
		cout << "Error: Unable to open output file " << ws2s(sOut) << endl;
		return false;
	}
	
	//Write out our fontResourceHeader
	frh.chars.count = lCharRecords.size();
	frh.chars.offset = sizeof(fontResourceHeader);
	frh.kerns.count = lKerningRecords.size();
	frh.kerns.offset = frh.chars.offset + sizeof(fontCharacterRecord) * frh.chars.count;
	fwrite(&frh, 1, sizeof(fontResourceHeader), f);
	
	//Write out our font characters
	for(list<fontCharacterRecord>::iterator i = lCharRecords.begin(); i != lCharRecords.end(); i++)
		fwrite(&(*i), 1, sizeof(fontCharacterRecord), f);
	
	//Write out our font kernings
	for(list<fontKerningRecord>::iterator i = lKerningRecords.begin(); i != lKerningRecords.end(); i++)
		fwrite(&(*i), 1, sizeof(fontKerningRecord), f);

	//Done
	fclose(f);
	return true;
}












