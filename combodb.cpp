#include "pakDataTypes.h"

bool comboDBToXML(const wchar_t* cFilename);
{
	//Open the file
	FILE* f = _wfopen(cFilename, TEXT("rb"));
	if(f == NULL)
	{
		cout << "Error: could not open " << ws2s(cFilename) << " for reading." << endl;
		return false;
	}
	
	//Read in the manifest
	comboHeader ch;
	if(fread(&ch, 1, sizeof(comboHeader), f) != sizeof(comboHeader))
	{
		cout << "Error: unable to read comboHeader from file " << ws2s(cFilename) << endl;
		fclose(f);
		return false;
	}
	
	//Read in combos
	vector<comboRecord> vCombos;
	for(int i = 0; i < ch.combos.count; i++)
	{
		comboRecord cr;
		if(fread(&cr, 1, sizeof(comboRecord), f) != sizeof(comboRecord))
		{
			cout << "Error: unable to read comboRecord from file " << ws2s(cFilename) << endl;
			fclose(f);
			return false;
		}
		vCombos.push_back(cr);
	}
	
	//Read in 
}

bool XMLToComboDB(const wchar_t* cFilename)
{
}