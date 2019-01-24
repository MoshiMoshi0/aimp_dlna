#include "stdafx.h"
#include "AimpDlnaPlugin.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaAlbumArtProvider.h"
#include "AimpDlnaOptionsDialog.h"

Plugin* Plugin::singleton = nullptr;
HMODULE Plugin::Module = NULL;
const wstring Plugin::Id = L"AimpDlna";

HRESULT WINAPI Plugin::Initialize(IAIMPCore *Core) {
	core = Core;

	if (FAILED(AimpUtils::Initialize(core))) {
		Finalize();
		return E_FAIL;
	}

	if (FAILED(AimpHttp::Initialize(core))) {
		Finalize();
		return E_FAIL;
	}
	
	if (FAILED(Config::Initialize(core))) {
		Finalize();
		return E_FAIL;
	}

	auto logConfig = "plist:.level=" + to_string(Config::LogLevel) + ";.handlers=ConsoleHandler;.ConsoleHandler.filter=56";
	NPT_LogManager::GetDefault().Configure(logConfig.c_str());

	if (FAILED(core->RegisterExtension(IID_IAIMPServiceOptionsDialog, static_cast<IAIMPOptionsDialogFrame*>(new AimpDlnaOptionsDialog(core))))) {
		Finalize();
		return E_FAIL;
	}

	if (FAILED(core->RegisterExtension(IID_IAIMPServiceMusicLibrary, static_cast<IAIMPMLExtensionDataStorage*>(new AimpDlnaDataStorage())))) {
		Finalize();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT WINAPI Plugin::Finalize() {
	AimpUtils::Finalize();
	AimpHttp::Finalize();
	Config::Finalize();

	if (core) {
		core->Release();
		core = nullptr;
	}
	
	finalized = true;
	return S_OK;
}