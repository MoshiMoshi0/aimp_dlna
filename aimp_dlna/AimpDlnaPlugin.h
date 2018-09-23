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

	~Plugin() {
		if (!finalized)
			Finalize();

		singleton = nullptr;
	}

	PWCHAR WINAPI InfoGet(int index) {
		switch (index) {
		case AIMP_PLUGIN_INFO_NAME:	             return const_cast<PWCHAR>(L"DLNA Client");
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