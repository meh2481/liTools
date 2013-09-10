//Vector and Matrix classes (complete with operations, as opposed to the basic structs in pakDataTypes.h) for use in matrix.cpp
//Modified from Mygod's example C# program. Thanks, Mygod!

#include <cmath>
#include <cfloat>
#include <cstdio>
#include <string>
#include <sstream>
#include <tinyxml2.h>
#include "pakDataTypes.h"
using namespace std;

class Vector;
class Matrix;	//Forward-declare these for function that depends on both, that both depend on
Vector multiply(Vector v1, Matrix matrix);

const double Epsilon = 0.001;
const double Tolerance = 0.00000000000000022204460492503131;
const double PI = 3.1415926535897932384626433832795;

double RadianToDegree(double radian)
{
	return radian / PI * 180;
}

double DegreeToRadian(double degree)
{
	return degree * PI / 180;
}

class Vector
{
private:
	Vector(){};	//No default constructor
	
public:

	Vector(double x, double y)
	{
		X = x;
		Y = y;
	}
	
	Vector(const Vector& copy)
	{
		X = copy.X;
		Y = copy.Y;
	}

	double X, Y;
	
	Vector operator -()
	{
		Vector v(X, Y);
		return v;
	}
	
	Vector operator +(Vector v2)
	{
		Vector v(X + v2.X, Y + v2.Y);
		return v;
	}
	
	Vector operator -(Vector v2)
	{
		Vector v(X - v2.X, Y - v2.Y);
		return v;
	}
	
	Vector operator =(const Vector& v)
	{
		X = v.X;
		Y = v.Y;
	}
	
	Vector operator *(double value)
	{
		Vector v(X * value, Y * value);
		return v;
	}
	
	double operator *(Vector v2)
	{
		return X * v2.X + Y * v2.Y;
	}
	
	double CrossProduct(Vector v1, Vector v2)
	{
		return v1.X * v2.Y - v1.Y * v2.X;
	}
	
	Vector operator /(double value)
	{
		Vector v(X / value, Y / value);
	}

	double LengthSquared()
	{ 
		return X * X + Y * Y;
	}
	
	double Length()
	{ 
		return sqrt(LengthSquared());
	}
};


class Matrix
{
private:
	Matrix(){};
	
public:
	Matrix(double a, double b, double c, double d, double e, double f)
	{
		_11 = a;
		_12 = b;
		_13 = c;
		_21 = d;
		_22 = e;
		_23 = f;
	}

	double _11, _12, _13, _21, _22, _23;

	// these are assumed constants and can't be modified
	const static double _31 = 0;
	const static double _32 = 0;
	const static double _33 = 1;
	
	void set(double a, double b, double c, double d, double e, double f)
	{
		_11 = a;
		_12 = b;
		_13 = c;
		_21 = d;
		_22 = e;
		_23 = f;
	}

	Matrix operator *(Matrix b)
	{
		// the simplest O(n^3) naive matrix multiplication, the _3y items has been replaced by the constants
		// more details here: http://en.wikipedia.org/wiki/Matrix_multiplication#Algorithms_for_efficient_matrix_multiplication
		Matrix m(_11 * b._11 + _12 * b._21, _11 * b._12 + _12 * b._22, _11 * b._13 + _12 * b._23 + _13,
				 _21 * b._11 + _22 * b._21, _21 * b._12 + _22 * b._22, _21 * b._13 + _22 * b._23 + _23);
		return m;
	}
	
	Matrix operator=(const Matrix& m)
	{
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
	}
	
	Matrix RotateTransform(double theta)
	{
		double radian = DegreeToRadian(theta), cosine = cos(radian), sine = sin(radian);
		Matrix m(cosine, -sine, 0,
				 sine, cosine, 0);
		return m;
	}
	
	Matrix ScaleTransform(double scale_x, double scale_y)
	{
		Matrix m(scale_x, 0, 0, 
				 0, scale_y, 0);
		return m;
	}
	
	Matrix SkewTransform(double angle_x, double angle_y)
	{
		double skew_x = tan(DegreeToRadian(angle_x)), skew_y = tan(DegreeToRadian(angle_y));
		Matrix m(1, skew_x, 0,
				 skew_y, 1, 0);
		return m;
	}
	
	Matrix TranslateTransform(double offset_x, double offset_y)
	{
		Matrix m(1, 0, offset_x,
				 0, 1, offset_y);
		return m;
	}
	
	Matrix DoNothingTransform()
	{
		Matrix m(1, 0, 0,
				 0, 1, 0);
		return m;
	}

	double GetAngle(Vector v1, Vector v2)
	{
		return atan2(v1.CrossProduct(v1, v2), v1 * v2);
	}

	void WriteMatrix()
	{
		printf("%g\t%g\t%g\n%g\t%g\t%g\n", _11, _12, _13, _21, _22, _23);
	}
	
	void GetTransforms(XMLElement* parent, XMLDocument* doc)
	{
		// This method is based on the code in this topic:
		// http://social.msdn.microsoft.com/Forums/silverlight/en-US/f85aa39b-7f11-4861-a273-2cfdbe12ffe8/convert-a-matrixtransform-into-transformgroup
		Vector xMul(1.0, 0.0);
		Vector yMul(0.0, 1.0);
		Vector newX(0,0);
		Vector newY(0,0);
		newX = multiply(xMul, (*this));
		newY = multiply(yMul, (*this));
		double scaleX = newX.Length(), scaleY = newY.Length(), skewX = 0, skewY = 0, theta = 0;
		bool newXValid = (newX.LengthSquared() > Tolerance), newYValid = (newY.LengthSquared() > Tolerance);
		double angleX = 0.0, angleY = 0.0;
		if (newXValid)
			angleX = GetAngle(xMul, newX);
		if (newYValid)
			angleY = GetAngle(yMul, newY);
		if (newXValid && newYValid && abs(newX.CrossProduct(newX, newY)) <= Tolerance)   // newX and newY are valid and collinear
		{
			Vector axis = (abs(angleX) <= abs(angleY)) ? newX : newY;  // pick the axis that is more close to y=x
			skewX = skewY = PI / 4;
			scaleX *= 0.70710678118654752440084436210485;  // sqrt(2) / 2
			scaleY *= 0.70710678118654752440084436210485;
			Vector _45deg(1.0,1.0);
			theta = GetAngle(_45deg, axis);
			if (newX * axis < 0.0)
				scaleX = -scaleX;
			if (newY * axis < 0.0)
				scaleY = -scaleY;
		}
		else if (newXValid || newYValid)
		{
			if (!newYValid || newXValid && abs(angleX) <= abs(angleY))
			{
				Vector newXUnit = newX / scaleX;
				double xUnitDouble = newY * newXUnit;
				scaleY = (newY - newXUnit * xUnitDouble).Length();
				theta = angleX;
				if (newYValid)  // && newXValid
				{
					double angle = GetAngle(newY, newX);
					if (angle > 0.0)
					{
						scaleY = -scaleY;
						skewX = angle - PI / 2;
					}
					else 
						skewX = angle + PI / 2;
				}
			}
			else
			{
				Vector newYUnit = newY / scaleY;
				double yUnitDouble = newX * newYUnit;
				scaleX = (newX - newYUnit * yUnitDouble).Length();
				theta = angleY;
				if (newXValid)  // && newYValid
				{
					double angle = GetAngle(newY, newX);
					if (angle > 0.0)
					{
						scaleX = -scaleX;
						skewY = angle - PI / 2;
					}
					else skewY = angle + PI / 2;
				}
			}
		}
		XMLElement* transforms = doc->NewElement("transforms");
		if (abs(scaleX - 1) > Epsilon || abs(scaleY - 1) > Epsilon)
		{
			XMLElement* scale = doc->NewElement("scale");
			ostringstream oss;
			oss << scaleX << ", " << scaleY;
			scale->SetAttribute("factor", oss.str().c_str());
			transforms->InsertEndChild(scale);
		}
		if (abs(skewX) > Epsilon || abs(skewY) > Epsilon)
		{
			XMLElement* skew = doc->NewElement("skew");
			ostringstream oss;
			oss << RadianToDegree(skewX) << ", " << RadianToDegree(skewY);
			skew->SetAttribute("angle", oss.str().c_str());
			transforms->InsertEndChild(skew);
		}
		if (abs(theta) > Epsilon) 
		{
			XMLElement* rotate = doc->NewElement("rotate");
			rotate->SetAttribute("angle", RadianToDegree(theta));
			transforms->InsertEndChild(rotate);
		}
		if (abs(_13) > Epsilon || abs(_23) > Epsilon)
		{
			XMLElement* translate = doc->NewElement("translate");
			ostringstream oss;
			oss << _13 << ", " << _23;
			translate->SetAttribute("offset", oss.str().c_str());
			transforms->InsertEndChild(translate);
		}
		parent->InsertEndChild(transforms);
	}
	
	Matrix ParseTransforms(XMLElement* root)
	{
		Matrix transform = DoNothingTransform();
		for(XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
		{
			Matrix matrix(0,0,0,0,0,0);
			string s = child->Name();
			if(s == "rotate")
			{
				double angle = 0.0;
				child->QueryDoubleAttribute("angle", &angle);
				matrix = RotateTransform(angle);
			}
			else if(s == "scale")
			{
				string sVec = stripCommas(child->Attribute("factor"));
				istringstream iss(sVec);
				double x, y;
				iss >> x >> y;
				matrix = ScaleTransform(x, y);
			}
			else if(s == "skew")
			{	
				string sVec = stripCommas(child->Attribute("angle"));
				istringstream iss(sVec);
				double x, y;
				iss >> x >> y;
				matrix = SkewTransform(x, y);
			}
			else if(s == "translate")
			{
				string sVec = stripCommas(child->Attribute("offset"));
				istringstream iss(sVec);
				double x, y;
				iss >> x >> y;
				matrix = TranslateTransform(x, y);
			}
			else if(s == "matrix")
			{
				string smat = stripCommas(child->Attribute("matrix"));
				istringstream iss(smat);
				iss >> matrix._11 >> matrix._12 >> matrix._13 >> matrix._21 >> matrix._22 >> matrix._23;
			}
			else
			{
				cout << "Unknown element name: " << s << endl;
			}
			transform = matrix * transform;
		}
		return transform;
	}
};

Vector multiply(Vector v1, Matrix matrix)   // applying transform
{
	Vector v(matrix._11 * v1.X + matrix._12 * v1.Y,
			 matrix._21 * v1.X + matrix._22 * v1.Y);
	return v;
}