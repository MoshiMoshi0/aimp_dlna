#include "stdafx.h"
#include "AimpDlnaPlugin.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaAlbumArtProvider.h"

Plugin* Plugin::singleton = nullptr;
const wstring Plugin::Id = L"AimpDlna";

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