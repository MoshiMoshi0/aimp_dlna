#include "stdafx.h"
#include "AimpDlnaPlugin.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaAlbumArtProvider.h"

Plugin* Plugin::singleton = nullptr;
const wstring Plugin::Id = L"AimpDlna";

HRESULT WINAPI Plugin::Initialize(IAIMPCore *Core) {
	core = Core;

	if (FAILED(AimpUtils::Initialize(core))) {
		Finalize();
		return E_FAIL;
	}
	
	if (FAILED(Config::Initialize(core))) {
		Finalize();
		return E_FAIL;
	}

	auto logConfig = "plist:.level=" + to_string(Config::DebugLevel) + ";.handlers=ConsoleHandler;.ConsoleHandler.filter=56";
	NPT_LogManager::GetDefault().Configure(logConfig.c_str());

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
	AimpUtils::Finalize();
	Config::Finalize();

	if (core) {
		core->Release();
		core = nullptr;
	}
	
	finalized = true;
	return S_OK;
}