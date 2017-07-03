#ifndef FRM_MAIN_H
#define FRM_MAIN_H

#include <windows.h>


namespace ClipDemoClient {
class FrmMain {
public:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	FrmMain() = default;
	FrmMain(const FrmMain&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	FrmMain& operator=(const FrmMain&) = delete;

	/* Getters / Setters -------------------------------------------------------------------------------- */
	HWND GetForm();

	/* Functions ---------------------------------------------------------------------------------------- */
	void Show();
	LRESULT WinProc(UINT msg, WPARAM wp, LPARAM lp);

private:
	/* Static Functions --------------------------------------------------------------------------------- */
	static LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	/* Functions ---------------------------------------------------------------------------------------- */
	void Draw();
	void OnBtnVerificateClicked();
	bool Validate(LPCTSTR txtMagStr, int* pAccountId);

	/* Variables ---------------------------------------------------------------------------------------- */
	HWND frm;
	HWND txtMag;
	HWND txtCompany;
	HWND txtNameOnCard;
	HWND txtCardNumber;
	HWND txtAccountId;

	HWND btnVerificate;
	HWND btnClear;

	WNDPROC defaultEditWndProc;
};

}

#endif // !FRM_MAIN_H

