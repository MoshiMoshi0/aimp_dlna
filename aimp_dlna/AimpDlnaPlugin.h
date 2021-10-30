#pragma once

#include "stdafx.h"

using namespace std;

class Plugin : public IUnknownInterfaceImpl<IAIMPPlugin> {
private:
	static Plugin* singleton;

	bool finalized{ false };
	IAIMPCore* core;

	Plugin() : core(nullptr) {}

public:
	static HMODULE Module;
	static const wstring Id;

	static Plugin* instance() {
		if (!singleton)
			singleton = new Plugin();

		return singleton;
	}

	static wstring GetVersionString() {
		wchar_t szModPath[MAX_PATH] = { '\0' };
		GetModuleFileName(Plugin::Module, szModPath, MAX_PATH);
		DWORD dwHandle;
		DWORD dwSize = GetFileVersionInfoSize(szModPath, &dwHandle);

		wstring result = L"0.0.0.0";
		if (dwSize > 0) {
			BYTE* pbBuf = static_cast<BYTE*>(_malloca(dwSize));
			if (pbBuf != nullptr && GetFileVersionInfo(szModPath, 0, dwSize, pbBuf)) {
				UINT uiSize;
				VS_FIXEDFILEINFO *verInfo = NULL;
				if (VerQueryValue(pbBuf, L"\\", reinterpret_cast<LPVOID*>(&verInfo), &uiSize)) {
					auto major = HIWORD(verInfo->dwProductVersionMS);
					auto minor = LOWORD(verInfo->dwProductVersionMS);
					auto build = HIWORD(verInfo->dwProductVersionLS);
					auto patch = LOWORD(verInfo->dwProductVersionLS);

					result = to_wstring(major) + L"." + to_wstring(minor) + L"." + to_wstring(build) + L"." + to_wstring(patch);
				}
			}

			_freea(pbBuf);
		}

		return result;
	}

	~Plugin() {
		if (!finalized)
			Finalize();

		singleton = nullptr;
	}

	PWCHAR WINAPI InfoGet(int index) {
		switch (index) {
			case AIMP_PLUGIN_INFO_NAME:	             return const_cast<PWCHAR>(StringUtils::ToWideCharArray(L"DLNA Client v" + Plugin::GetVersionString()));
			case AIMP_PLUGIN_INFO_AUTHOR:            return const_cast<PWCHAR>(L"MoshiMoshi");
			case AIMP_PLUGIN_INFO_SHORT_DESCRIPTION: return const_cast<PWCHAR>(L"Allows AIMP to browse and play media from DLNA servers");
			default:
				break;
		}
		return nullptr;
	}

	DWORD WINAPI InfoGetCategories() {
		return AIMP_PLUGIN_CATEGORY_ADDONS;
	}

	HRESULT WINAPI Initialize(IAIMPCore* Core);
	HRESULT WINAPI Finalize();

	void WINAPI SystemNotification(int NotifyID, IUnknown* Data) {}
};