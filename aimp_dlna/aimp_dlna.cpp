#include "stdafx.h"
#include "aimp_dlna.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaAlbumArtProvider.h"

Plugin* Plugin::singleton = nullptr;

HRESULT WINAPI Plugin::Initialize(IAIMPCore *Core) {
	core = Core;

	AimpUtils::Initialize(core);

	if (FAILED(core->QueryInterface(IID_IAIMPServiceMUI, reinterpret_cast<void**>(&muiService)))) {
		Finalize();
		return E_FAIL;
	}
	
	if (FAILED(core->RegisterExtension(IID_IAIMPServiceMusicLibrary, static_cast<IAIMPMLExtensionDataStorage*>(AimpDlnaDataStorage::instance())))) {
		Finalize();
		return E_FAIL;
	}	
	
	if (FAILED(core->RegisterExtension(IID_IAIMPServiceAlbumArt, static_cast<IAIMPExtensionAlbumArtProvider*>(AimpDlnaAlbumArtProvider::instance())))) {
		Finalize();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT WINAPI Plugin::Finalize() {
	if (muiService) {
		muiService->Release();
		muiService = nullptr;
	}

	if (core) {
		core->Release();
		core = nullptr;
	}
	
	finalized = true;
	return S_OK;
}

wstring Plugin::Lang(const wstring &Key, int Part) {
	wstring ret;
	if (!muiService)
		return ret;

	IAIMPString *value = nullptr;
	if (Part > -1) {
		if (SUCCEEDED(muiService->GetValuePart(AimpString(Key), Part, &value))) {
			ret = value->GetData();
			value->Release();
		}
	} else {
		if (SUCCEEDED(muiService->GetValue(AimpString(Key), &value))) {
			ret = value->GetData();
			value->Release();
		}
	}
	return ret;
}