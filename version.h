#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
	static const char DATE[] = "22";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2025";
	static const char UBUNTU_VERSION_STYLE[] =  "25.10";

	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";

	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 1;
	static const long BUILD  = 14;
	static const long REVISION  = 71;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 16;
	#define RC_FILEVERSION 1,1,14,71
	#define RC_FILEVERSION_STRING "1, 1, 14, 71\0"
	static const char FULLVERSION_STRING [] = "1.1.14.71";

	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;

}
#endif //VERSION_H
