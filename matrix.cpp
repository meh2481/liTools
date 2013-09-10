//Matrix math functions for usability of XML
//Thanks to Mygod for providing!

#include "pakDataTypes.h"
#include "matrix.h"
#include <sstream>

#define PI 3.1415926535
#define EPSILON	0.001

void WriteTransforms(XMLElement* elemParent, XMLDocument* doc, mtx23 mat)
{
	Matrix m(mat._11, mat._12, mat._13, mat._21, mat._22, mat._23);
	m.GetTransforms(elemParent, doc);
	//cout << "Mat: " << mat._11 << " " << mat._12 << " " << mat._13 << " " << mat._21 << " " << mat._22 << " " << mat._23 << endl;
}

void ReadTransforms(XMLElement* elem, mtx23* mat)
{
	Matrix m(0,0,0,0,0,0);
	m = m.ParseTransforms(elem);
	mat->_11 = m._11;
	mat->_12 = m._12;
	mat->_13 = m._13;
	mat->_21 = m._21;
	mat->_22 = m._22;
	mat->_23 = m._23;
	
	//cout << "Mat: " << mat->_11 << " " << mat->_12 << " " << mat->_13 << " " << mat->_21 << " " << mat->_22 << " " << mat->_23 << endl;
}
