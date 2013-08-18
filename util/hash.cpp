//Print out hashed value of a string
#include "../pakDataTypes.h"


//Main program entry point
int main(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		wstring s = s2ws(argv[i]);
		cout << "Hashed value of \"" << argv[i] << "\" is " << hash(s) << endl;
	}
	return 0;
}