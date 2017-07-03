#include <stdio.h> 
#include <tchar.h>

#include "BizMain.h"
#include "AppCommon.h"


namespace ClipDemoClient {

/* Public Functions  -------------------------------------------------------------------------------- */
bool BizMain:: GetCard(int accountId, LPCTSTR magStr, LPTSTR companyName, LPTSTR nameOnCard, LPTSTR cardNumber) {
	bool ret = false;

	const char host[] = "s.apps.clip-card.com";
	const char user[] = "select_only";
	const char password[] = "";
	const char dbname[] = "clip_database_staging";
	int port = 51433;
	CHAR query[] = "select company_name, name, number from cards where account_id = ? and magnetic_back = ? and deleted_at is null;";

	// DB接続
	MYSQL* pMysql = mysql_init(0);
	if (mysql_real_connect(pMysql, host, user, password, dbname, 51433, NULL, 0) == NULL) {
		OutputDebugString(_T(mysql_error(pMysql)));
		OutputDebugString(_T("\n"));
		return false;
	}

	// パラメータ設定
	MYSQL_STMT* pStmt = mysql_stmt_init(pMysql);
	if (!pStmt) {
		mysql_close(pMysql);
		return false;
	}
	if (mysql_stmt_prepare(pStmt, query, static_cast<ULONG>(strlen(query)))) {
		mysql_close(pMysql);
		return false;
	}
	MYSQL_BIND binds[2];
	ULONG magStrLength;
	if (!SetInputParameters(pStmt, &accountId, magStr, binds, &magStrLength)) {
		mysql_close(pMysql);
		return false;
	}

	// SQL発行
	if (mysql_stmt_execute(pStmt)) {
		OutputDebugString(_T(mysql_error(pMysql)));
		OutputDebugString(_T("\n"));
		mysql_close(pMysql);
		return false;
	}

	// 取得結果格納用バッファ設定
	MYSQL_BIND result[3];
	ULONG length[3];
	my_bool isError[3];
	CHAR tmpCompanyName[AppCommon::LENGTH_OF_COMPANY_NAME];
	CHAR tmpNameOnCard[AppCommon::LENGTH_OF_NANE_ON_CARD];
	CHAR tmpCardNumber[AppCommon::LENGTH_OF_CARD_NUMBER];
	if (!SetOutputParameters(pStmt, result, length, isError, tmpCompanyName, tmpNameOnCard, tmpCardNumber)) {
		mysql_close(pMysql);
		return false;
	}

	// 1レコード目のデータを取得
	if (!mysql_stmt_fetch(pStmt)) {
		_stprintf_s(companyName, AppCommon::LENGTH_OF_COMPANY_NAME - 1, _T("%s"), tmpCompanyName);
		_stprintf_s(nameOnCard, AppCommon::LENGTH_OF_NANE_ON_CARD - 1, _T("%s"), tmpNameOnCard);
		_stprintf_s(cardNumber, AppCommon::LENGTH_OF_CARD_NUMBER - 1, _T("%s"), tmpCardNumber);
		ret = true;
	}

	// 切断
	mysql_close(pMysql);
	return ret;
}


/* Private Functions  ------------------------------------------------------------------------------- */
bool BizMain::SetInputParameters(MYSQL_STMT* pStmt, const int* pAccountId, LPCTSTR magStr, MYSQL_BIND binds[2], ULONG* pMagStrLength) {
	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = (int*)pAccountId;
	binds[0].is_null = 0;

	binds[1].buffer_type = MYSQL_TYPE_STRING;
	binds[1].buffer = (TCHAR*)magStr;
	binds[1].buffer_length = sizeof(TCHAR) * static_cast<ULONG>(_tcslen(magStr));
	binds[1].is_null = 0;
	*pMagStrLength = sizeof(TCHAR) * static_cast<ULONG>(_tcslen(magStr));
	binds[1].length = pMagStrLength;

	if (mysql_stmt_bind_param(pStmt, binds)) {
		// エラー処理
		OutputDebugString(_T(mysql_stmt_error(pStmt)));
		OutputDebugString(_T("\n"));
		return false;
	}
	return true;
}

bool BizMain::SetOutputParameters(MYSQL_STMT* pStmt, MYSQL_BIND results[3], ULONG lengths[3], my_bool isErrors[3], LPSTR companyName, LPSTR nameOnCard, LPSTR cardNumber) {
	results[0].buffer_type = MYSQL_TYPE_STRING;
	results[0].buffer = companyName;
	results[0].is_null = 0;
	results[0].buffer_length = sizeof(CHAR) * AppCommon::LENGTH_OF_COMPANY_NAME;
	results[0].length = &lengths[0];
	results[0].error = &isErrors[0];

	results[1].buffer_type = MYSQL_TYPE_STRING;
	results[1].buffer = nameOnCard;
	results[1].is_null = 0;
	results[1].buffer_length = sizeof(CHAR) * AppCommon::LENGTH_OF_NANE_ON_CARD;
	results[1].length = &lengths[1];
	results[1].error = &isErrors[1];

	results[2].buffer_type = MYSQL_TYPE_STRING;
	results[2].buffer = cardNumber;
	results[2].is_null = 0;
	results[2].buffer_length = sizeof(CHAR) * AppCommon::LENGTH_OF_CARD_NUMBER;
	results[2].length = &lengths[2];
	results[2].error = &isErrors[2];

	if (mysql_stmt_bind_result(pStmt, results)) {
		// エラー処理
		OutputDebugString(_T(mysql_stmt_error(pStmt)));
		OutputDebugString(_T("\n"));
		return false;
	}
	return true;
}

}