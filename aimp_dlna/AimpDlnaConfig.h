#pragma once

#include "AimpDlnaPlugin.h"

#define _KEY(value) Plugin::Id + L"\\" + L#value
#define _LOAD(type, target, defaultValue) \
	if(FAILED(configService->GetValueAs##type(AimpString(_KEY(target)), &target))){ \
		target = defaultValue; \
	}

#define _SAVE(type, source) configService->SetValueAs##type(AimpString(_KEY(source)), source);

class Config {
private:
	static IAIMPCore *core;
	static IAIMPServiceConfig* configService;

public:
	static int DebugLevel;

	static void Save() {
		_SAVE(Int32, DebugLevel);
		configService->FlushCache();
	}

	static void Load() {
		_LOAD(Int32, DebugLevel, NPT_LOG_LEVEL_OFF);
	}

	static HRESULT Initialize(IAIMPCore *Core) { 
		core = Core; 

		if (FAILED(core->QueryInterface(IID_IAIMPServiceConfig, reinterpret_cast<void**>(&configService)))) {
			return E_FAIL;
		}
		
		Load();
		Save();

		return S_OK;
	}

	static void Finalize() {
		if (configService) {
			configService->FlushCache();
			configService->Release();
			configService = nullptr;
		}
	}
};