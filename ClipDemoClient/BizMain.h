#ifndef BIZ_MAIN_H
#define BIZ_MAIN_H

#include <mysql.h>


namespace ClipDemoClient {

class BizMain {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	BizMain() = default;
	BizMain(const BizMain&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	BizMain& operator=(const BizMain&) = delete;
	
	/* Functions ---------------------------------------------------------------------------------------- */
	bool GetCard(int accountId, LPCTSTR magStr, LPTSTR companyName, LPTSTR nameOnCard, LPTSTR cardNumber);

private:
	/* Functions ---------------------------------------------------------------------------------------- */
	bool SetInputParameters(MYSQL_STMT* pStmt, const int* accountId, LPCTSTR magStr, MYSQL_BIND binds[2], ULONG* pMagStrLength);
	bool SetOutputParameters(MYSQL_STMT* pStmt, MYSQL_BIND results[3], ULONG lengths[3], my_bool isErrors[3], LPSTR companyName, LPSTR nameOnCard, LPSTR cardNumber);
};

}

#endif // !BIZ_MAIN_H
