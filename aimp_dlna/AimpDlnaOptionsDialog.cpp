#include "stdafx.h"
#include "AimpDlnaOptionsDialog.h"
#include "resource.h"

#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

#define WM_SUBCLASSINIT WM_USER + 1

HRESULT WINAPI AimpDlnaOptionsDialog::GetName(IAIMPString **S) {
	*S = AimpString(L"DLNA Client", true);
	return S_OK;
}

HWND WINAPI AimpDlnaOptionsDialog::CreateFrame(HWND ParentWnd) {
	handle = CreateDialogParam(Plugin::Module, MAKEINTRESOURCE(IDD_OPTIONSDIALOG), ParentWnd, DlgProc, (LPARAM)this);
	if (handle != NULL) {
		RECT R;
		GetWindowRect(ParentWnd, &R);
		OffsetRect(&R, -R.left, -R.top);
		SetWindowPos(handle, NULL, R.left, R.top, R.right - R.left, R.bottom - R.top, SWP_SHOWWINDOW);
		Notification(AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOCALIZATION);
		Notification(AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOAD);
	}

	return handle;
}

void WINAPI AimpDlnaOptionsDialog::DestroyFrame() {
	DestroyWindow(handle);
	handle = NULL;
}

void WINAPI AimpDlnaOptionsDialog::Notification(int ID) {
	static const int debugValues[9] = { NPT_LOG_LEVEL_OFF,	NPT_LOG_LEVEL_ALL, NPT_LOG_LEVEL_FATAL,
										NPT_LOG_LEVEL_SEVERE, NPT_LOG_LEVEL_WARNING, NPT_LOG_LEVEL_INFO,
										NPT_LOG_LEVEL_FINE, NPT_LOG_LEVEL_FINER, NPT_LOG_LEVEL_FINEST };
	switch (ID) {
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOCALIZATION:
			break;
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOAD: {
			SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_SETCURSEL, 0, 0);
			for (size_t i = 0; i < 9; i++) {
				if (debugValues[i] == Config::DebugLevel) {
					SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_SETCURSEL, i, 0);
					break;
				}
			}
			break;
		}
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_SAVE: {
			int selectedIndex = SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_GETCURSEL, 0, 0);
			Config::DebugLevel = debugValues[selectedIndex];
			Config::Save();
			break;
		}
	}
}

BOOL CALLBACK AimpDlnaOptionsDialog::DlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	static AimpDlnaOptionsDialog* dialog = nullptr;

	switch (Msg) {
		case WM_INITDIALOG: {
			dialog = reinterpret_cast<AimpDlnaOptionsDialog*>(lParam);

			InitCommonControls();
			SetWindowSubclass(GetDlgItem(hwnd, IDC_MAINFRAME), FrameProc, 0, lParam); SendDlgItemMessage(hwnd, IDC_MAINFRAME, WM_SUBCLASSINIT, 0, 0);
			SetWindowSubclass(GetDlgItem(hwnd, IDC_GROUPBOX_ADVANCED), GroupBoxProc, 0, lParam); SendDlgItemMessage(hwnd, IDC_GROUPBOX_ADVANCED, WM_SUBCLASSINIT, 0, 0);

			// INIT IDC_DEBUGCOMBOBOX
			{
				const int debugValues[9] = { NPT_LOG_LEVEL_OFF,	NPT_LOG_LEVEL_ALL, NPT_LOG_LEVEL_FATAL,
											 NPT_LOG_LEVEL_SEVERE, NPT_LOG_LEVEL_WARNING, NPT_LOG_LEVEL_INFO,
											 NPT_LOG_LEVEL_FINE, NPT_LOG_LEVEL_FINER, NPT_LOG_LEVEL_FINEST };
				const wstring debugNames[9] = { L"OFF\0", L"ALL\0", L"FATAL\0", L"SEVERE\0", L"WARNING\0", L"INFO\0", L"FINE\0", L"FINER\0", L"FINEST\0" };
				for (auto o : debugNames)
					SendDlgItemMessage(hwnd, IDC_COMBOBOX_DEBUG, CB_ADDSTRING, 1, (LPARAM)o.c_str());
			}


			break;
		}
		case WM_SIZE: {
			RECT rc, rc2;
			GetClientRect(hwnd, &rc);

			SetWindowPos(GetDlgItem(hwnd, IDC_MAINFRAME), NULL, rc.left, rc.top, rc.right, rc.bottom, 0);
			HWND group = GetDlgItem(hwnd, IDC_GROUPBOX_ADVANCED); GetClientRect(group, &rc2); SetWindowPos(group, NULL, 0, 0, rc.right - 21, rc2.bottom, SWP_NOMOVE | SWP_NOZORDER);
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_COMBOBOX_DEBUG:
					switch (HIWORD(wParam)) {
						case CBN_SELENDOK: { dialog->Dirty(); break; }
						default: break;
					}
				default: break;
			}
			break;
		case WM_DESTROY:
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK AimpDlnaOptionsDialog::FrameProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	static HBRUSH bgBrush;
	static HPEN penOuter;
	static HPEN penInner;
	static RECT rect;
	static RECT captionRect;
	static HFONT captionFont;
	static TRIVERTEX gradient[2];
	static wchar_t text[64];

	switch (uMsg) {
		case WM_SUBCLASSINIT: {
			GetWindowText(hwnd, text, 64);

			bgBrush = CreateSolidBrush(RGB(240, 240, 240));
			penOuter = CreatePen(PS_SOLID, 1, RGB(188, 188, 188));
			penInner = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			captionFont = CreateFont(13, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Tahoma");
			break;
		}
		case WM_SIZE: {
			GetClientRect(hwnd, &rect);

			captionRect = rect;
			captionRect.bottom = 21;

			gradient[0] = { captionRect.left + 1, captionRect.top + 1, 0x1200, 0xb100, 0x4b00, 0xffff }; // #0cb14b
			gradient[1] = { captionRect.right - 1, captionRect.bottom, 0x0b00, 0x9600, 0x4000, 0xffff }; // #0b9640
			return TRUE;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			SelectObject(hdc, bgBrush);

			// Outer frame
			SelectObject(hdc, penOuter);
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

			SelectObject(hdc, penInner);
			Rectangle(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1);

			// Caption
			SelectObject(hdc, penOuter);
			Rectangle(hdc, rect.left, rect.top, rect.right, captionRect.bottom + 1);

			GRADIENT_RECT r = { 0, 1 };
			GradientFill(hdc, gradient, 2, &r, 1, GRADIENT_FILL_RECT_V);

			SetTextColor(hdc, 0x00ffffff);
			SetBkMode(hdc, TRANSPARENT);

			SelectObject(hdc, captionFont);
			DrawText(hdc, text, wcslen(text), &captionRect, DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			EndPaint(hwnd, &ps);
			return TRUE;
		}
		case WM_DESTROY: {
			DeleteObject(bgBrush);
			DeleteObject(penOuter);
			DeleteObject(penInner);
			DeleteObject(captionFont);
			break;
		}
		case WM_NCDESTROY: {
			RemoveWindowSubclass(hwnd, FrameProc, uIdSubclass);
			break;
		}
	}

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK AimpDlnaOptionsDialog::GroupBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	static HBRUSH bgBrush;
	static HPEN penOuter;
	static HPEN penInner;
	static HFONT captionFont;

	switch (uMsg) {
		case WM_SUBCLASSINIT:
			bgBrush = CreateSolidBrush(RGB(240, 240, 240));
			penOuter = CreatePen(PS_SOLID, 1, RGB(188, 188, 188));
			penInner = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			captionFont = CreateFont(13, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Tahoma");
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));

			wchar_t text[64];
			GetWindowText(hWnd, text, 64);

			RECT rect;
			GetClientRect(hWnd, &rect);

			// Outer frame
			SelectObject(hdc, penOuter);
			Rectangle(hdc, rect.left, rect.top + 7, rect.right, rect.bottom);

			SelectObject(hdc, penInner);
			Rectangle(hdc, rect.left + 1, rect.top + 7 + 1, rect.right - 1, rect.bottom - 1);

			// Caption
			SetTextColor(hdc, RGB(0x80, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, captionFont);

			RECT captionRect(rect);
			captionRect.left += 5;

			SIZE textSize;
			GetTextExtentPoint32(hdc, text, wcslen(text), &textSize);
			LPtoDP(hdc, (POINT *)&textSize, 2);
			captionRect.right = captionRect.left + textSize.cx + 10;
			captionRect.bottom = captionRect.top + textSize.cy;

			FillRect(hdc, &captionRect, bgBrush);

			captionRect.left += 5;
			DrawText(hdc, text, wcslen(text), &captionRect, DT_NOCLIP | DT_LEFT | DT_TOP | DT_SINGLELINE);

			EndPaint(hWnd, &ps);
			return TRUE;
		}
		case WM_DESTROY:
			if (GetWindowLong(hWnd, GWL_ID) == IDC_GROUPBOX_ADVANCED) {
				DeleteObject(bgBrush);
				DeleteObject(penOuter);
				DeleteObject(penInner);
				DeleteObject(captionFont);
			}
			break;
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, GroupBoxProc, uIdSubclass);
			break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}