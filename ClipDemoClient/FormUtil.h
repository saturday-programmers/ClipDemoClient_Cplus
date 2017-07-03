#ifndef FORM_UTIL_H
#define FORM_UTIL_H

#include <windows.h>


namespace ClipDemoClient {
class FormUtil {
public:
	/* Static Functions --------------------------------------------------------------------------------- */
	static HWND CreateNewWindow();
	static void DrawPanel(HDC hdc, int left, int top, int right, int bottom);
	static void ShowText(HDC hdc, LPCTSTR str, const POINT& rPoint);
	static HWND CreateTextBox(HWND hWnd, const POINT& rPoint, int width, bool isReadOnly);
	static HWND CreateButton(HWND hWnd, const POINT& rPoint, int width, int height, LPCTSTR caption);
	static HFONT CreateFont(LPCTSTR fontName, int fontSize);

private:
	/* Constructor / Destructor ------------------------------------------------------------------------- */
	FormUtil() = delete;
	FormUtil(const FormUtil&) = delete;

	/* Operator Overloads ------------------------------------------------------------------------------- */
	FormUtil& operator=(const FormUtil&) = delete;

	/* Constants ---------------------------------------------------------------------------------------- */
	static const int WINDOW_DEFAULT_HEIGHT = 630;
	static const int WINDOW_DEFAULT_WIDTH = 1120;
};
}

#endif // !FORM_UTIL_H
