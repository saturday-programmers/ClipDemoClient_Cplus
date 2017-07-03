#ifndef APP_COMMON_H
#define APP_COMMON_H

#include <tchar.h>


namespace ClipDemoClient {

class AppCommon {
public:
	/* Constants ---------------------------------------------------------------------------------------- */
	static const TCHAR* TITLE;
	static const int LENGTH_OF_MAG = 79;
	static const int LENGTH_OF_COMPANY_NAME = 100;
	static const int LENGTH_OF_NANE_ON_CARD = 100;
	static const int LENGTH_OF_CARD_NUMBER = 100;

private:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	AppCommon() = delete;
	AppCommon(const AppCommon&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	AppCommon& operator=(const AppCommon&) = delete;
};

}

#endif

