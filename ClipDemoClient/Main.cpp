#include <locale.h>
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <vector>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static void Draw(HWND hWnd);
static void DrawPanel(HDC hdc);
static void ShowText(HDC hdc, LPSTR str, POINT point);
static HWND CreateTextBox(HWND hWnd, POINT point, int width, bool isReadOnly);
static HWND CreateButton(HWND hWnd, POINT point, int width, int height, TCHAR caption[]);
static HFONT SetFont(LPCTSTR fontName, int fontSize);
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

static const TCHAR* TITLE = TEXT("CLIP Demo Client");
static const int WINDOW_DEFAULT_HEIGHT = 630;
static const int WINDOW_DEFAULT_WIDTH = 1120;

static HWND g_txtMag;
static HWND g_txtCompany;
static HWND g_txtName;
static HWND g_txtNumber;
static HWND g_txtAccountId;

static HWND g_btnVerificate;
static HWND g_btnClear;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/) {
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	_tsetlocale(LC_ALL, "Japanese");            //ロケール（地域言語）を日本語でセット

	HWND hWnd;
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

	hWnd = CreateWindow(
		TITLE,
		TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,		// x
		0,		// y
		WINDOW_DEFAULT_WIDTH + dw,		// Width
		WINDOW_DEFAULT_HEIGHT + dh,		// Height
		NULL, NULL, hInstance, NULL
	);
	if (!hWnd) return 0;

	g_txtMag = CreateTextBox(hWnd, POINT{ 150, 100 }, 700, false);
	g_txtCompany = CreateTextBox(hWnd, POINT{ 200, 250 }, 300, true);
	g_txtName = CreateTextBox(hWnd, POINT{ 200, 300 }, 300, true);
	g_txtNumber = CreateTextBox(hWnd, POINT{ 200, 350 }, 300, true);
	g_txtAccountId = CreateTextBox(hWnd, POINT{ 200, 500 }, 50, false);

	g_btnVerificate = CreateButton(hWnd, POINT{ 870, 100 }, 53, 24, _T("照合"));
	g_btnClear = CreateButton(hWnd, POINT{ 930, 100 }, 65, 24, _T("クリア"));

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;	
}

static void Draw(HWND hWnd) {
	HDC hdc;
	PAINTSTRUCT paint;
	hdc = BeginPaint(hWnd, &paint);

	HFONT hFont = SetFont(_T("Myrica M"), 16);
	SelectObject(hdc, hFont);

	DrawPanel(hdc);
	ShowText(hdc, _T("磁気情報"), POINT{ 50, 100 });
	ShowText(hdc, _T("カード会社"), POINT{ 100, 250 });
	ShowText(hdc, _T("登録者名"), POINT{ 100, 300 });
	ShowText(hdc, _T("カード番号"), POINT{ 100, 350 });
	ShowText(hdc, _T("アカウントID"), POINT{ 50, 500 });

	DeleteObject(hFont);
	EndPaint(hWnd, &paint);
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

static void ShowText(HDC hdc, LPSTR str, POINT point) {
	TextOut(hdc, point.x, point.y, str, _tcslen(str));
}

static HWND CreateTextBox(HWND hWnd, POINT point, int width, bool isReadOnly) {
	static int windowId = 0;
	windowId++;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hEdit = CreateWindow(
		_T("EDIT"),			// ウィンドウクラス名
		NULL,				// キャプション
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,	// スタイル指定	
		point.x,			// x
		point.y,			// y
		width,				// 幅
		20,					// 高さ
		hWnd,				// 親ウィンドウ
		(HMENU)windowId,	// メニューハンドルまたは子ウィンドウID
		hInstance,			// インスタンスハンドル
		NULL				// その他の作成データ
	);

	HFONT hFont = SetFont(_T("Myrica M"), 16);
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(false, 0));

	if (isReadOnly) {
		SendMessage(hEdit, EM_SETREADONLY, 1, 0);
	}
	return hEdit;
}

static HWND CreateButton(HWND hWnd, POINT point, int width, int height, TCHAR caption[]) {
	static int windowId = 10;
	windowId++;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hButton = CreateWindow(
						_T("BUTTON"),		// ウィンドウクラス名
						caption,			// キャプション
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	// スタイル指定	
						point.x,			// x
						point.y, 			// y
						width, 				// 幅
						height,				// 高さ
						hWnd, 				// 親ウィンドウ
						(HMENU)windowId, 	// メニューハンドルまたは子ウィンドウID
						hInstance, 			// インスタンスハンドル
						NULL				// その他の作成データ
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
		case WM_PAINT:
			Draw(hWnd);
			return 0;
		case WM_CTLCOLOREDIT:
		case  WM_CTLCOLORSTATIC:
		{
			HWND hw = (HWND)lp;
			if (hw == g_txtMag || hw == g_txtAccountId) {
				SetBkColor((HDC)wp, RGB(245, 245, 245));
			} else if (hw == g_txtCompany || hw == g_txtName || hw == g_txtNumber) {
				SetBkColor((HDC)wp, RGB(176, 196, 222));
			}

			return 0;
		}
		case WM_COMMAND:
		{
			HWND hw = (HWND)lp;
			if (hw == g_btnVerificate) {

			} else if (hw == g_btnClear) {
				SetWindowText(g_txtMag, _T(""));
				SetWindowText(g_txtCompany, _T(""));
				SetWindowText(g_txtName, _T(""));
				SetWindowText(g_txtNumber, _T(""));
				SetWindowText(g_txtAccountId, _T(""));
			}
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			break;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

