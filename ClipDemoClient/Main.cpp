#include <locale.h>
#include <windows.h>
#include <mmsystem.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "AppCommon.h"
#include "FrmMain.h"


namespace ClipDemoClient{
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

FrmMain* pFrmMain;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/) {
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);

	using namespace ClipDemoClient;

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
	winc.lpszClassName = AppCommon::TITLE;
	// Windowsの登録
	if (!RegisterClass(&winc)) return 0;

	pFrmMain = new FrmMain();
	pFrmMain->Show();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete pFrmMain;
	return (int)msg.wParam;	
}

namespace ClipDemoClient {

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if (hWnd == pFrmMain->GetForm()) {
		return pFrmMain->WinProc(msg, wp, lp);
	} else {
		return DefWindowProc(hWnd, msg, wp, lp);
	}
}

}
