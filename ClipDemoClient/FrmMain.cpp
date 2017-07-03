#include <tchar.h>

#include "FrmMain.h"
#include "AppCommon.h"
#include "FormUtil.h"
#include "BizMain.h"


namespace ClipDemoClient {

namespace {
/* Static Variables --------------------------------------------------------------------------------- */
FrmMain* instance;
}


/* Getters / Setters -------------------------------------------------------------------------------- */
HWND FrmMain::GetForm() {
	return this->frm;
}


/* Public Functions  -------------------------------------------------------------------------------- */
void FrmMain::Show() {
	HWND hWind = FormUtil::CreateNewWindow();
	this->frm = hWind;

	this->txtMag = FormUtil::CreateTextBox(hWind, POINT{ 150, 100 }, 700, false);
	this->txtCompany = FormUtil::CreateTextBox(hWind, POINT{ 200, 250 }, 300, true);
	this->txtNameOnCard = FormUtil::CreateTextBox(hWind, POINT{ 200, 300 }, 300, true);
	this->txtCardNumber = FormUtil::CreateTextBox(hWind, POINT{ 200, 350 }, 300, true);
	this->txtAccountId = FormUtil::CreateTextBox(hWind, POINT{ 200, 500 }, 50, false);
	SetWindowText(this->txtAccountId, _T("10"));
	SetFocus(this->txtMag);
	// 磁気情報のテキストボックスはEnterキー押下検出のためサブクラス化
	this->defaultEditWndProc = (WNDPROC)SetWindowLongPtr(this->txtMag, GWLP_WNDPROC, (LONG_PTR)FrmMain::EditWindowProc);

	this->btnVerificate = FormUtil::CreateButton(hWind, POINT{ 870, 100 }, 53, 24, _T("照合"));
	this->btnClear = FormUtil::CreateButton(hWind, POINT{ 930, 100 }, 65, 24, _T("クリア"));
	instance = this;
}

LRESULT FrmMain::WinProc(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_CREATE:
			SetFocus(this->txtMag);
			return 0;
		case WM_PAINT:
			Draw();
			return 0;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			HWND hw = (HWND)lp;
			if (hw == this->txtMag || hw == this->txtAccountId) {
				SetBkColor((HDC)wp, RGB(245, 245, 245));
			} else if (hw == this->txtCompany || hw == this->txtNameOnCard || hw == this->txtCardNumber) {
				SetBkColor((HDC)wp, RGB(176, 196, 222));
			}

			return 0;
		}
		case WM_COMMAND:
		{
			HWND hw = (HWND)lp;
			if (hw == this->btnVerificate) {
				OnBtnVerificateClicked();
			} else if (hw == this->btnClear) {
				SetWindowText(this->txtMag, _T(""));
				SetWindowText(this->txtCompany, _T(""));
				SetWindowText(this->txtNameOnCard, _T(""));
				SetWindowText(this->txtCardNumber, _T(""));
				SetWindowText(this->txtAccountId, _T(""));
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
	return DefWindowProc(this->frm, msg, wp, lp);
}


/* Static Private Functions ------------------------------------------------------------------------- */
LRESULT CALLBACK FrmMain::EditWindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	// Enterキー押下時は照合ボタン押下時と同様の処理を行う
	if (msg == WM_KEYDOWN && wp == VK_RETURN) {
		instance->OnBtnVerificateClicked();
		return 0;
	}
	return CallWindowProc(instance->defaultEditWndProc, hWnd, msg, wp, lp);
}


/* Private Functions  ------------------------------------------------------------------------------- */
void FrmMain::Draw() {
	PAINTSTRUCT paint;
	HDC hdc = BeginPaint(this->frm, &paint);

	HFONT hFont = FormUtil::CreateFont(_T("Myrica M"), 16);
	SelectObject(hdc, hFont);

	FormUtil::DrawPanel(hdc, 75, 200, 1000, 450);
	FormUtil::ShowText(hdc, _T("磁気情報"), POINT{ 50, 100 });
	FormUtil::ShowText(hdc, _T("カード会社"), POINT{ 100, 250 });
	FormUtil::ShowText(hdc, _T("登録者名"), POINT{ 100, 300 });
	FormUtil::ShowText(hdc, _T("カード番号"), POINT{ 100, 350 });
	FormUtil::ShowText(hdc, _T("アカウントID"), POINT{ 50, 500 });

	DeleteObject(hFont);
	EndPaint(this->frm, &paint);
}

void FrmMain::OnBtnVerificateClicked() {
	TCHAR txtMagStr[AppCommon::LENGTH_OF_MAG];
	ZeroMemory(&txtMagStr, sizeof(txtMagStr));
	GetWindowText(this->txtMag, (LPTSTR)txtMagStr, sizeof(txtMagStr) / sizeof(TCHAR));

	int accountId;
	if (!Validate(txtMagStr, &accountId)) {
		SetWindowText(this->txtCompany, _T(""));
		SetWindowText(this->txtNameOnCard, _T(""));
		SetWindowText(this->txtCardNumber, _T(""));
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
	TCHAR magStr[AppCommon::LENGTH_OF_MAG];
	ZeroMemory(&magStr, sizeof(magStr));
	memcpy_s(magStr, sizeof(magStr), txtMagStr + startPos, sizeof(TCHAR) * (endPos - startPos + 1));

	// カード情報取得
	TCHAR companyName[AppCommon::LENGTH_OF_COMPANY_NAME];
	ZeroMemory(&companyName, sizeof(companyName));
	TCHAR name[AppCommon::LENGTH_OF_NANE_ON_CARD];
	ZeroMemory(&name, sizeof(name));
	TCHAR number[AppCommon::LENGTH_OF_CARD_NUMBER];
	ZeroMemory(&number, sizeof(number));
	BizMain biz;
	bool isSuccess = biz.GetCard(accountId, magStr, companyName, name, number);
	if (isSuccess) {
		SetWindowText(this->txtCompany, companyName);
		SetWindowText(this->txtNameOnCard, name);
		SetWindowText(this->txtCardNumber, number);
	} else {
		MessageBox(this->frm, _T("DB接続に失敗しました。"), AppCommon::TITLE, MB_OK | MB_ICONERROR);
	}
}

bool FrmMain::Validate(LPCTSTR txtMagStr, int* pAccountId) {
	bool ret = false;
	*pAccountId = INT_MAX;

	TCHAR accountIdStr[3];
	ZeroMemory(&accountIdStr, sizeof(accountIdStr));
	GetWindowText(this->txtAccountId, (LPTSTR)accountIdStr, 3);

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
			MessageBox(this->frm, _T("アカウントIDが正しくありません。"), AppCommon::TITLE, MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return ret;
}
}