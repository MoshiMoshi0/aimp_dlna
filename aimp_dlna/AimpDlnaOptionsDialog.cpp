#include "stdafx.h"
#include "AimpDlnaOptionsDialog.h"
#include "resource.h"

#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

#define WM_SUBCLASSINIT WM_USER + 1

#define A_TOP		0x01 
#define A_BOTTOM	0x02
#define A_LEFT		0x04
#define A_RIGHT		0x08
#define A_HFILL		(A_LEFT | A_RIGHT)
#define A_VFILL		(A_TOP | A_BOTTOM)
#define A_FILL		(A_VFILL | A_HFILL)
#define AF_MOVE		0x01
#define AF_SIZE		0x02
#define AF_FLIP		0x04

#define BITTEST(x, f) ((x & f) == f)

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
	static const int logValues[9] = { NPT_LOG_LEVEL_OFF,	NPT_LOG_LEVEL_ALL, NPT_LOG_LEVEL_FATAL,
									  NPT_LOG_LEVEL_SEVERE, NPT_LOG_LEVEL_WARNING, NPT_LOG_LEVEL_INFO,
									  NPT_LOG_LEVEL_FINE, NPT_LOG_LEVEL_FINER, NPT_LOG_LEVEL_FINEST };
	switch (ID) {
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOCALIZATION: {
			SetDlgItemText(handle, IDC_GROUPBOX_GENERAL,		AimpUtils::Lang(L"AimpDlna.Options\\General").c_str());
			SetDlgItemText(handle, IDC_GROUPBOX_ADVANCED,		AimpUtils::Lang(L"AimpDlna.Options\\Advanced").c_str());
			SetDlgItemText(handle, IDC_LABEL_LOGLEVEL,			AimpUtils::Lang(L"AimpDlna.Options\\LogLevel").c_str());
			SetDlgItemText(handle, IDC_LABEL_RESTART,			AimpUtils::Lang(L"AimpDlna.Options\\Restart").c_str());
			SetDlgItemText(handle, IDC_LABEL_BLACKLIST,			AimpUtils::Lang(L"AimpDlna.Options\\Blacklist").c_str());
			SetDlgItemText(handle, IDC_CHECKBOX_SCANSTOP,		AimpUtils::Lang(L"AimpDlna.Options\\ScanStop").c_str());
			SetDlgItemText(handle, IDC_LABEL_SCANDURATION,		AimpUtils::Lang(L"AimpDlna.Options\\ScanDuration").c_str());
			SetDlgItemText(handle, IDC_LABEL_MS1,				AimpUtils::Lang(L"AimpDlna.Options\\MS").c_str());
			SetDlgItemText(handle, IDC_LABEL_MS2,				AimpUtils::Lang(L"AimpDlna.Options\\MS").c_str());
			SetDlgItemText(handle, IDC_LABEL_DELAYFOR,			AimpUtils::Lang(L"AimpDlna.Options\\DelayFor").c_str());
			break;
		}
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_LOAD: {
			SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_SETCURSEL, 0, 0);
			for (size_t i = 0; i < 9; i++) {
				if (logValues[i] == Config::LogLevel) {
					SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_SETCURSEL, i, 0);
					break;
				}
			}

			SendDlgItemMessage(handle, IDC_CHECKBOX_SCANSTOP, BM_SETCHECK, Config::ScanStop, 0);
			EnableWindow(GetDlgItem(handle, IDC_EDIT_STOPDELAY), (bool)Config::ScanStop);
			EnableWindow(GetDlgItem(handle, IDC_LABEL_DELAYFOR), (bool)Config::ScanStop);
			EnableWindow(GetDlgItem(handle, IDC_LABEL_MS2), (bool)Config::ScanStop);

			SetDlgItemText(handle, IDC_EDIT_SCANDURATION, to_wstring(Config::ScanDuration).c_str());
			SetDlgItemText(handle, IDC_EDIT_STOPDELAY, to_wstring(Config::StopDelay).c_str());
			SetDlgItemText(handle, IDC_EDIT_BLACKLIST, StringUtils::Replace(Config::UuidBlacklist, L"|", L"\r\n").c_str());
			break;
		}
		case AIMP_SERVICE_OPTIONSDIALOG_NOTIFICATION_SAVE: {
			TCHAR buffer[1024];
			GetDlgItemText(handle, IDC_EDIT_SCANDURATION, buffer, 64);
			Config::ScanDuration = _wtoi(buffer);
			GetDlgItemText(handle, IDC_EDIT_STOPDELAY, buffer, 64);
			Config::StopDelay = _wtoi(buffer);

			Config::LogLevel = logValues[SendDlgItemMessage(handle, IDC_COMBOBOX_DEBUG, CB_GETCURSEL, 0, 0)];
			Config::ScanStop = SendDlgItemMessage(handle, IDC_CHECKBOX_SCANSTOP, BM_GETCHECK, 0, 0);

			GetDlgItemText(handle, IDC_EDIT_BLACKLIST, buffer, 1024);
			Config::UuidBlacklist = StringUtils::Replace(buffer, L"\r\n", L"|");

			Config::Save();
			break;
		}
	}
}

inline void AimpDlnaOptionsDialog::MoveControl(HWND hwnd, int x, int y) {
	SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

inline void AimpDlnaOptionsDialog::SizeControl(HWND hwnd, int w, int h) {
	SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void AimpDlnaOptionsDialog::AlignControl(HWND hwnd, int source, int target, int margin, int anchor, int flags) {
	RECT sr = { 0 }, tr = { 0 };
	HWND sh = GetDlgItem(hwnd, source);

	if (target != NULL) {
		GetWindowRect(GetDlgItem(hwnd, target), &tr);

		ScreenToClient(hwnd, (POINT*)&tr);
		ScreenToClient(hwnd, (POINT*)&tr + 1);
	} else {
		GetClientRect(hwnd, &tr);
	}

	for (size_t i = 0; i < 4 && anchor > 0; i++) {
		GetWindowRect(sh, &sr);
		ScreenToClient(hwnd, (POINT*)&sr);
		ScreenToClient(hwnd, (POINT*)&sr + 1);

		if (BITTEST(anchor, A_LEFT)) {
			auto nx = (!BITTEST(flags, AF_FLIP) ? tr.left : tr.right) + margin;
			auto nw = (!BITTEST(flags, AF_FLIP) ? sr.right : sr.left) - nx;
			
			MoveControl(sh, nx, sr.top);
			if (BITTEST(flags, AF_SIZE))
				SizeControl(sh, nw, sr.bottom - sr.top);

			anchor &= ~A_LEFT;
		} else if (BITTEST(anchor, A_TOP)) {
			auto ny = (!BITTEST(flags, AF_FLIP) ? tr.top : tr.bottom) + margin;;
			auto nh = (!BITTEST(flags, AF_FLIP) ? sr.bottom : sr.top) - ny;

			MoveControl(sh, sr.left, ny);
			if (BITTEST(flags, AF_SIZE))
				SizeControl(sh, sr.right - sr.left, nh);

			anchor &= ~A_TOP;
		} else if (BITTEST(anchor, A_RIGHT)) {
			auto nx = (!BITTEST(flags, AF_FLIP) ? tr.right : tr.left) - margin;

			if(BITTEST(flags, AF_MOVE))
				MoveControl(sh, nx - (sr.right - sr.left), sr.top);
			if (BITTEST(flags, AF_SIZE))
				SizeControl(sh, nx - sr.left, sr.bottom - sr.top);

			anchor &= ~A_RIGHT;
		} else if (BITTEST(anchor, A_BOTTOM)) {
			auto nw = (!BITTEST(flags, AF_FLIP) ? tr.bottom : tr.top) - margin;

			if (BITTEST(flags, AF_MOVE))
				MoveControl(sh, sr.left, nw - (sr.bottom - sr.top));
			if (BITTEST(flags, AF_SIZE))
				SizeControl(sh, sr.right - sr.left, nw - sr.top);

			anchor &= ~A_BOTTOM;
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
			SetWindowSubclass(GetDlgItem(hwnd, IDC_GROUPBOX_GENERAL), GroupBoxProc, 0, lParam); SendDlgItemMessage(hwnd, IDC_GROUPBOX_GENERAL, WM_SUBCLASSINIT, 0, 0);
			SetWindowSubclass(GetDlgItem(hwnd, IDC_GROUPBOX_ADVANCED), GroupBoxProc, 0, lParam); SendDlgItemMessage(hwnd, IDC_GROUPBOX_ADVANCED, WM_SUBCLASSINIT, 0, 0);

			const wstring logNames[9] = { L"OFF", L"ALL", L"FATAL", L"SEVERE", L"WARNING", L"INFO", L"FINE", L"FINER", L"FINEST" };
			for (auto o : logNames)
				SendDlgItemMessage(hwnd, IDC_COMBOBOX_DEBUG, CB_ADDSTRING, 1, (LPARAM)o.c_str());

			break;
		}
		case WM_SIZE: {
			AlignControl(hwnd, IDC_MAINFRAME, NULL, 0, A_FILL, AF_SIZE);

			// Align IDC_GROUPBOX_GENERAL
			AlignControl(hwnd, IDC_GROUPBOX_GENERAL,	IDC_MAINFRAME,				10, A_HFILL, AF_SIZE);
			AlignControl(hwnd, IDC_GROUPBOX_GENERAL,	IDC_MAINFRAME,				21 + 10, A_TOP, AF_SIZE);

			// Align to IDC_GROUPBOX_GENERAL
			AlignControl(hwnd, IDC_LABEL_SCANDURATION,	IDC_GROUPBOX_GENERAL,		10, A_LEFT, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_SCANDURATION,	IDC_GROUPBOX_GENERAL,		15, A_TOP, AF_MOVE);
			AlignControl(hwnd, IDC_EDIT_SCANDURATION,	IDC_GROUPBOX_GENERAL,		12, A_TOP, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_MS1,			IDC_GROUPBOX_GENERAL,		15, A_TOP, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_MS1,			IDC_GROUPBOX_GENERAL,		10, A_RIGHT, AF_MOVE);
			
			AlignControl(hwnd, IDC_CHECKBOX_SCANSTOP,	IDC_GROUPBOX_GENERAL,		10, A_LEFT | A_RIGHT, AF_SIZE);

			AlignControl(hwnd, IDC_LABEL_DELAYFOR,		IDC_GROUPBOX_GENERAL,		10, A_LEFT, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_MS2,			IDC_GROUPBOX_GENERAL,		10, A_RIGHT, AF_MOVE);

			// Align IDC_GROUPBOX_GENERAL children
			AlignControl(hwnd, IDC_CHECKBOX_SCANSTOP,	IDC_LABEL_SCANDURATION,		5, A_TOP, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_LABEL_DELAYFOR,		IDC_CHECKBOX_SCANSTOP,		5, A_TOP, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_EDIT_STOPDELAY,		IDC_CHECKBOX_SCANSTOP,		2, A_TOP, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_LABEL_MS2,			IDC_CHECKBOX_SCANSTOP,		5, A_TOP, AF_MOVE | AF_FLIP);

			AlignControl(hwnd, IDC_EDIT_SCANDURATION,	IDC_LABEL_MS1,				2, A_RIGHT, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_EDIT_STOPDELAY,		IDC_LABEL_MS2,				2, A_RIGHT, AF_MOVE | AF_FLIP);

			// Align IDC_GROUPBOX_GENERAL
			AlignControl(hwnd, IDC_GROUPBOX_GENERAL,	IDC_LABEL_DELAYFOR,			-10, A_BOTTOM, AF_SIZE);



			// Align IDC_GROUPBOX_ADVANCED
			AlignControl(hwnd, IDC_GROUPBOX_ADVANCED,	IDC_MAINFRAME,				10, A_HFILL | A_BOTTOM, AF_SIZE);

			// Align to IDC_GROUPBOX_ADVANCED
			AlignControl(hwnd, IDC_LABEL_RESTART,		IDC_GROUPBOX_ADVANCED,		5, A_BOTTOM | A_RIGHT, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_LOGLEVEL,		IDC_GROUPBOX_ADVANCED,		10, A_LEFT, AF_MOVE);
			AlignControl(hwnd, IDC_LABEL_BLACKLIST,		IDC_GROUPBOX_ADVANCED,		10, A_LEFT, AF_MOVE);
			AlignControl(hwnd, IDC_EDIT_BLACKLIST,		IDC_GROUPBOX_ADVANCED,		10, A_LEFT, AF_MOVE);

			// Align IDC_GROUPBOX_ADVANCED children
			AlignControl(hwnd, IDC_EDIT_BLACKLIST,		IDC_LABEL_RESTART,			5, A_BOTTOM, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_LABEL_BLACKLIST,		IDC_EDIT_BLACKLIST,			5, A_BOTTOM, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_LABEL_LOGLEVEL,		IDC_LABEL_BLACKLIST,		5, A_BOTTOM, AF_MOVE | AF_FLIP);
			AlignControl(hwnd, IDC_COMBOBOX_DEBUG,		IDC_LABEL_BLACKLIST,		1, A_BOTTOM, AF_MOVE | AF_FLIP);

			// Align IDC_GROUPBOX_ADVANCED
			AlignControl(hwnd, IDC_GROUPBOX_ADVANCED,	IDC_LABEL_LOGLEVEL,			-15, A_TOP, AF_SIZE);
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_COMBOBOX_DEBUG: {
					switch (HIWORD(wParam)) {
						case CBN_SELENDOK: { dialog->Dirty(); break; }
						default: break;
					}
					break;
				}
				case IDC_EDIT_SCANDURATION:
				case IDC_EDIT_STOPDELAY:
				case IDC_EDIT_BLACKLIST: {
					if (HIWORD(wParam) != EN_SETFOCUS && HIWORD(wParam) != EN_KILLFOCUS) {
						dialog->Dirty();
					}
					break;
				}
				case IDC_CHECKBOX_SCANSTOP: {
					bool checked = SendDlgItemMessage(hwnd, IDC_CHECKBOX_SCANSTOP, BM_GETCHECK, 0, 0);

					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_STOPDELAY), checked);
					EnableWindow(GetDlgItem(hwnd, IDC_LABEL_DELAYFOR), checked);
					EnableWindow(GetDlgItem(hwnd, IDC_LABEL_MS2), checked);

					dialog->Dirty(); 
					break;
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