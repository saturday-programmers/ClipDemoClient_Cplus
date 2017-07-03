#include <tchar.h>

#include "FormUtil.h"
#include "AppCommon.h"


namespace ClipDemoClient {

/* Static Public Functions -------------------------------------------------------------------------- */
HWND FormUtil::CreateNewWindow() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	int dh = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) * 2;
	int dw = GetSystemMetrics(SM_CXFRAME) * 2;

	HWND hWnd = CreateWindow(
					AppCommon::TITLE,
					AppCommon::TITLE,
					WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					0,		// x
					0,		// y
					WINDOW_DEFAULT_WIDTH + dw,		// Width
					WINDOW_DEFAULT_HEIGHT + dh,		// Height
					NULL, 
					NULL, 
					hInstance,
					NULL
				);
	return hWnd;
}

void FormUtil::DrawPanel(HDC hdc, int left, int top, int right, int bottom) {
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hOldPen = static_cast<HPEN>(SelectObject(hdc, hPen));
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));
	Rectangle(hdc, left, top, right, bottom);
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void FormUtil::ShowText(HDC hdc, LPCTSTR str, const POINT& rPoint) {
	TextOut(hdc, rPoint.x, rPoint.y, str, static_cast<int>(_tcslen(str)));
}

HWND FormUtil::CreateTextBox(HWND hWnd, const POINT& rPoint, int width, bool isReadOnly) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hTextBox = CreateWindow(
						_T("EDIT"),										// ウィンドウクラス名
						NULL,											// キャプション
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,	// スタイル指定	
						rPoint.x,										// x
						rPoint.y,										// y
						width,											// 幅
						20,												// 高さ
						hWnd,											// 親ウィンドウ
						NULL,											// メニューハンドルまたは子ウィンドウID
						hInstance,										// インスタンスハンドル
						NULL											// その他の作成データ
					);

	HFONT hFont = CreateFont(_T("Myrica M"), 16);
	SendMessage(hTextBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(false, 0));

	if (isReadOnly) {
		SendMessage(hTextBox, EM_SETREADONLY, 1, 0);
	}
	return hTextBox;
}

HWND FormUtil::CreateButton(HWND hWnd, const POINT& rPoint, int width, int height, LPCTSTR caption) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hButton = CreateWindow(
						_T("BUTTON"),							// ウィンドウクラス名
						caption,								// キャプション
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	// スタイル指定	
						rPoint.x,								// x
						rPoint.y, 								// y
						width, 									// 幅
						height,									// 高さ
						hWnd, 									// 親ウィンドウ
						NULL, 									// メニューハンドルまたは子ウィンドウID
						hInstance, 								// インスタンスハンドル
						NULL									// その他の作成データ
					);

	HFONT hFont = CreateFont(_T("Myrica M"), 16);
	SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(false, 0));
	return hButton;
}

HFONT FormUtil::CreateFont(LPCTSTR fontName, int fontSize) {
	HFONT hFont = ::CreateFont(
						fontSize,					// フォント高さ
						0,							// 文字幅
						0,							// テキストの角度
						0,							// ベースラインとｘ軸との角度
						FW_REGULAR,					// フォントの重さ（太さ）
						FALSE,						// イタリック体
						FALSE,						// アンダーライン
						FALSE,						// 打ち消し線
						SHIFTJIS_CHARSET,			// 文字セット
						OUT_DEFAULT_PRECIS,			// 出力精度
						CLIP_DEFAULT_PRECIS,		// クリッピング精度
						PROOF_QUALITY,				// 出力品質
						FIXED_PITCH | FF_MODERN,	// ピッチとファミリー
						fontName					// 書体名
					);
	return hFont;
}

}