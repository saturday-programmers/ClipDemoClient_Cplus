#include <locale.h>
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <vector>
#include <ctype.h>
#include <mysql.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static void Draw();
static void DrawPanel(HDC hdc);
static void ShowText(HDC hdc, LPCTSTR str, const POINT& pPoint);
static HWND CreateTextBox(const POINT& rPoint, int width, bool isReadOnly);
static HWND CreateButton(const POINT& rPoint, int width, int height, LPCTSTR caption);
static HFONT SetFont(LPCTSTR fontName, int fontSize);
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
static void OnBtnVerificateClicked();
static bool Validate(LPCTSTR txtMagStr, int* pAccountId);
static bool GetCard(int accountId, LPCTSTR magStr, LPTSTR companyName, LPTSTR nameOnCard, LPTSTR cardNumber);

static const TCHAR* TITLE = TEXT("CLIP Demo Client");
static const int WINDOW_DEFAULT_HEIGHT = 630;
static const int WINDOW_DEFAULT_WIDTH = 1120;
static const int LENGTH_OF_MAG = 79;
static const int LENGTH_OF_COMPANY_NAME = 100;
static const int LENGTH_OF_NANE_ON_CARD = 100;
static const int LENGTH_OF_CARD_NUMBER = 100;

static HWND g_frmMain;
static HWND g_txtMag;
static HWND g_txtCompany;
static HWND g_txtNameOnCard;
static HWND g_txtCardNumber;
static HWND g_txtAccountId;

static HWND g_btnVerificate;
static HWND g_btnClear;

static WNDPROC g_defEditWndProc;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/) {
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	_tsetlocale(LC_ALL, "Japanese");            //ロケール（地域言語）を日本語でセット

	WNDCLASS winc;
	
	// Windows情報の設定
	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra = winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TITLE;
	// Windowsの登録
	if (!RegisterClass(&winc)) return 0;

	// Windowsの生成
	int dh = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) * 2;
	int dw = GetSystemMetrics(SM_CXFRAME) * 2;

	g_frmMain = CreateWindow(
		TITLE,
		TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,		// x
		0,		// y
		WINDOW_DEFAULT_WIDTH + dw,		// Width
		WINDOW_DEFAULT_HEIGHT + dh,		// Height
		NULL, NULL, hInstance, NULL
	);
	if (!g_frmMain) return 0;

	g_txtMag = CreateTextBox(POINT{ 150, 100 }, 700, false);
	g_txtCompany = CreateTextBox(POINT{ 200, 250 }, 300, true);
	g_txtNameOnCard = CreateTextBox(POINT{ 200, 300 }, 300, true);
	g_txtCardNumber = CreateTextBox(POINT{ 200, 350 }, 300, true);
	g_txtAccountId = CreateTextBox(POINT{ 200, 500 }, 50, false);
	SetWindowText(g_txtAccountId, _T("10"));
	SetFocus(g_txtMag);
	// 磁気情報のテキストボックスはEnterキー押下検出のためサブクラス化
	g_defEditWndProc = (WNDPROC)SetWindowLongPtr(g_txtMag, GWLP_WNDPROC, (LONG_PTR)EditWindowProc);

	g_btnVerificate = CreateButton(POINT{ 870, 100 }, 53, 24, _T("照合"));
	g_btnClear = CreateButton(POINT{ 930, 100 }, 65, 24, _T("クリア"));

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;	
}

static void Draw() {
	PAINTSTRUCT paint;
	HDC hdc = BeginPaint(g_frmMain, &paint);

	HFONT hFont = SetFont(_T("Myrica M"), 16);
	SelectObject(hdc, hFont);

	DrawPanel(hdc);
	ShowText(hdc, _T("磁気情報"), POINT{ 50, 100 });
	ShowText(hdc, _T("カード会社"), POINT{ 100, 250 });
	ShowText(hdc, _T("登録者名"), POINT{ 100, 300 });
	ShowText(hdc, _T("カード番号"), POINT{ 100, 350 });
	ShowText(hdc, _T("アカウントID"), POINT{ 50, 500 });

	DeleteObject(hFont);
	EndPaint(g_frmMain, &paint);
}

static void DrawPanel(HDC hdc) {
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));
	Rectangle(hdc, 75, 200, 1000, 450);
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

static void ShowText(HDC hdc, LPCTSTR str, const POINT& rPoint) {
	TextOut(hdc, rPoint.x, rPoint.y, str, static_cast<int>(_tcslen(str)));
}

static HWND CreateTextBox(const POINT& rPoint, int width, bool isReadOnly) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hTextBox = CreateWindow(
						_T("EDIT"),										// ウィンドウクラス名
						NULL,											// キャプション
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,	// スタイル指定	
						rPoint.x,										// x
						rPoint.y,										// y
						width,											// 幅
						20,												// 高さ
						g_frmMain,										// 親ウィンドウ
						NULL,				// メニューハンドルまたは子ウィンドウID
						hInstance,										// インスタンスハンドル
						NULL											// その他の作成データ
					);

	HFONT hFont = SetFont(_T("Myrica M"), 16);
	SendMessage(hTextBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(false, 0));

	if (isReadOnly) {
		SendMessage(hTextBox, EM_SETREADONLY, 1, 0);
	}
	return hTextBox;
}

static HWND CreateButton(const POINT& rPoint, int width, int height, LPCTSTR caption) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hButton = CreateWindow(
						_T("BUTTON"),							// ウィンドウクラス名
						caption,								// キャプション
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	// スタイル指定	
						rPoint.x,								// x
						rPoint.y, 								// y
						width, 									// 幅
						height,									// 高さ
						g_frmMain, 								// 親ウィンドウ
						NULL, 		// メニューハンドルまたは子ウィンドウID
						hInstance, 								// インスタンスハンドル
						NULL									// その他の作成データ
					);

	HFONT hFont = SetFont(_T("Myrica M"), 16);
	SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(false, 0));
	return hButton;
}

static HFONT SetFont(LPCTSTR fontName, int fontSize) {
	HFONT hFont = CreateFont(
					fontSize,				// フォント高さ
					0,						// 文字幅
					0,		                // テキストの角度
					0,						// ベースラインとｘ軸との角度
					FW_REGULAR,				// フォントの重さ（太さ）
					FALSE,					// イタリック体
					FALSE,					// アンダーライン
					FALSE,					// 打ち消し線
					SHIFTJIS_CHARSET,		// 文字セット
					OUT_DEFAULT_PRECIS,		// 出力精度
					CLIP_DEFAULT_PRECIS,	// クリッピング精度
					PROOF_QUALITY,			// 出力品質
					FIXED_PITCH | FF_MODERN,// ピッチとファミリー
					fontName				// 書体名
				);
	return hFont;
}

//-------------------------------------------------------------
//
//	メッセージ処理
//
//-------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_CREATE:
			SetFocus(g_txtMag);
			return 0;
		case WM_PAINT:
			Draw();
			return 0;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			HWND hw = (HWND)lp;
			if (hw == g_txtMag || hw == g_txtAccountId) {
				SetBkColor((HDC)wp, RGB(245, 245, 245));
			} else if (hw == g_txtCompany || hw == g_txtNameOnCard || hw == g_txtCardNumber) {
				SetBkColor((HDC)wp, RGB(176, 196, 222));
			}

			return 0;
		}
		case WM_COMMAND:
		{
			HWND hw = (HWND)lp;
			if (hw == g_btnVerificate) {
				OnBtnVerificateClicked();
			} else if (hw == g_btnClear) {
				SetWindowText(g_txtMag, _T(""));
				SetWindowText(g_txtCompany, _T(""));
				SetWindowText(g_txtNameOnCard, _T(""));
				SetWindowText(g_txtCardNumber, _T(""));
				SetWindowText(g_txtAccountId, _T(""));
			}
			return 0;
		}
		case WM_CHAR:
			// Enterキー押下時は照合ボタン押下時と同様の処理を行う
			if (wp == VK_RETURN) {
				OnBtnVerificateClicked();
				return 0;
			} else {
				break;
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			break;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	// Enterキー押下時は照合ボタン押下時と同様の処理を行う
	if (msg == WM_KEYDOWN && wp == VK_RETURN) {
		OnBtnVerificateClicked();
		return 0;
	}
	return CallWindowProc(g_defEditWndProc, hWnd, msg, wp, lp);
}

static void OnBtnVerificateClicked() {
	TCHAR txtMagStr[LENGTH_OF_MAG];
	ZeroMemory(&txtMagStr, sizeof(txtMagStr));
	GetWindowText(g_txtMag, (LPTSTR)txtMagStr, sizeof(txtMagStr) / sizeof(TCHAR));

	int accountId;
	if (!Validate(txtMagStr, &accountId)) {
		SetWindowText(g_txtCompany, _T(""));
		SetWindowText(g_txtNameOnCard, _T(""));
		SetWindowText(g_txtCardNumber, _T(""));
		return;
	}

	/* *～*内を取り出し */
	int startPos = INT_MAX;
	for (int i = 0; i < _tcslen(txtMagStr); ++i) {
		if (txtMagStr[i] == _T('*')) {
			startPos = i + 1;
			break;
		}
	}

	int endPos = INT_MAX;
	for (int i = static_cast<int>(_tcslen(txtMagStr)) - 1; i > 0; --i) {
		if (txtMagStr[i] == _T('*')) {
			endPos = i - 1;
			break;
		}
	}
	TCHAR magStr[LENGTH_OF_MAG];
	ZeroMemory(&magStr, sizeof(magStr));
	memcpy_s(magStr, sizeof(magStr), txtMagStr + startPos, sizeof(TCHAR) * (endPos - startPos + 1));

	// カード情報取得
	TCHAR companyName[LENGTH_OF_COMPANY_NAME];
	ZeroMemory(&companyName, sizeof(companyName));
	TCHAR name[LENGTH_OF_NANE_ON_CARD];
	ZeroMemory(&name, sizeof(name));
	TCHAR number[LENGTH_OF_CARD_NUMBER];
	ZeroMemory(&number, sizeof(number));
	bool isSuccess = GetCard(accountId, magStr, companyName, name, number);
	if (isSuccess) {
		SetWindowText(g_txtCompany, companyName);
		SetWindowText(g_txtNameOnCard, name);
		SetWindowText(g_txtCardNumber, number);
	} else {
		MessageBox(g_frmMain, _T("DB接続に失敗しました。"), TITLE, MB_OK | MB_ICONERROR);
	}
}

static bool Validate(LPCTSTR txtMagStr, int* pAccountId) {
	bool ret = false;
	*pAccountId = INT_MAX;

	TCHAR accountIdStr[3];
	ZeroMemory(&accountIdStr, sizeof(accountIdStr));
	GetWindowText(g_txtAccountId, (LPTSTR)accountIdStr, 3);

	size_t magLength = _tcslen(txtMagStr);
	if (magLength > 60 && txtMagStr[magLength - 1] == _T('*')) {
		size_t accountIdLength = _tcslen(accountIdStr);
		ret = (accountIdLength > 0);
		for (size_t i = 0; ret && i < accountIdLength; ++i) {
			ret = (_istdigit(accountIdStr[i]) != 0);
		}

		if (ret) {
			*pAccountId = _ttoi(accountIdStr);
		} else {
			MessageBox(g_frmMain, _T("アカウントIDが正しくありません。"), TITLE, MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return ret;
}

static bool GetCard(int accountId, LPCTSTR magStr, LPTSTR companyName, LPTSTR nameOnCard, LPTSTR cardNumber) {
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
	MYSQL_BIND bind[2];
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = &accountId;
	bind[0].is_null = 0;

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (TCHAR*)magStr;
	bind[1].buffer_length = static_cast<ULONG>(_tcslen(magStr));
	bind[1].is_null = 0;
	ULONG strLen = static_cast<ULONG>(_tcslen(magStr));
	bind[1].length = &strLen;

	if (mysql_stmt_bind_param(pStmt, bind)) {
		// エラー処理
		OutputDebugString(_T(mysql_stmt_error(pStmt)));
		OutputDebugString(_T("\n"));
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
	result[0].buffer_type = MYSQL_TYPE_STRING;
	CHAR tmpCompanyName[LENGTH_OF_COMPANY_NAME];
	result[0].buffer = &tmpCompanyName;
	result[0].is_null = 0;
	result[0].buffer_length = sizeof(tmpCompanyName);
	result[0].length = &length[0];
	result[0].error = &isError[0];

	result[1].buffer_type = MYSQL_TYPE_STRING;
	CHAR tmpNameOnCard[LENGTH_OF_NANE_ON_CARD];
	result[1].buffer = &tmpNameOnCard;
	result[1].is_null = 0;
	result[1].buffer_length = sizeof(tmpNameOnCard);
	result[1].length = &length[1];
	result[1].error = &isError[1];

	result[2].buffer_type = MYSQL_TYPE_STRING;
	CHAR tmpCardNumber[LENGTH_OF_CARD_NUMBER];
	result[2].buffer = &tmpCardNumber;
	result[2].is_null = 0;
	result[2].buffer_length = sizeof(tmpCardNumber);
	result[2].length = &length[2];
	result[2].error = &isError[2];

	if (mysql_stmt_bind_result(pStmt, result)) {
		// エラー処理
		OutputDebugString(_T(mysql_stmt_error(pStmt)));
		OutputDebugString(_T("\n"));
		mysql_close(pMysql);
	}

	// 1レコード目のデータを取得
	if (!mysql_stmt_fetch(pStmt)) {
		_stprintf_s(companyName, LENGTH_OF_COMPANY_NAME - 1, _T("%s"), tmpCompanyName);
		_stprintf_s(nameOnCard, LENGTH_OF_NANE_ON_CARD - 1, _T("%s"), tmpNameOnCard);
		_stprintf_s(cardNumber, LENGTH_OF_CARD_NUMBER - 1, _T("%s"), tmpCardNumber);
		ret = true;
	}
	
	// 切断
	mysql_close(pMysql);
	return ret;
}
