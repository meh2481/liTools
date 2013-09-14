#include "pakDataTypes.h"
#include <sstream>
using namespace std;

void writeVec2(XMLElement* elem, string sAttributeName, vec2 vec)
{
	//TODO
}

string stripCommas(string s)
{
    //Replace all ',' characters with ' '
    for(int i = 0; ; i++)
    {
        size_t iPos = s.find(',', i);
        if(iPos == s.npos)
            break;  //Done

        s.replace(iPos, 1, " ");
    }
    return s;
}

vec2 pointFromString(string s)
{
    s = stripCommas(s);

    //Now, parse
    istringstream iss(s);
    vec2 pt;
    if(!(iss >> pt.x >> pt.y))
		pt.x = pt.y = 0;
    return pt;
}

string RGBToString(f32 r, f32 g, f32 b)
{
	ostringstream oss;
	oss << r << ", " << g << ", " << b;
	return oss.str();
}

void RGBFromString(f32* r, f32* g, f32* b, string s)
{
	s = stripCommas(s);
	istringstream iss(s);
	if(!(iss >> *r >> *g >> *b))
		*r = *g = *b = 0;
}

void readVec2(XMLElement* elem, string sAttributeName, vec2* vec)
{
	const char* c = elem->Attribute(sAttributeName.c_str());
	if(c == NULL)
	{
		cout << "Err in readVec2" << endl;
		vec->x = vec->y = 0;
		return;
	}
	vec2 pt = pointFromString(c);
	vec->x = pt.x;
	vec->y = pt.y;
}
