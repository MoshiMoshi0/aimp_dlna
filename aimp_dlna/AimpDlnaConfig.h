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
	static int LogLevel;
	static int ScanDuration;
	static int ScanStop;
	static int StopDelay;
	static wstring UuidBlacklist;
	static int UseCache;
	static int CacheDepth;

	static void Save() {
		_SAVE(Int32, LogLevel);
		_SAVE(Int32, ScanDuration);
		_SAVE(Int32, ScanStop);
		_SAVE(Int32, StopDelay);

		AimpString UuidBlacklist(Config::UuidBlacklist);
		_SAVE(String, UuidBlacklist);

		_SAVE(Int32, UseCache);
		_SAVE(Int32, CacheDepth);

		configService->FlushCache();
	}

	static void Load() {
		_LOAD(Int32, LogLevel, NPT_LOG_LEVEL_OFF);
		_LOAD(Int32, ScanDuration, 2000);
		_LOAD(Int32, ScanStop, 1);
		_LOAD(Int32, StopDelay, 250);

		IAIMPString* UuidBlacklist;
		_LOAD(String, UuidBlacklist, nullptr);
		if (UuidBlacklist != nullptr) {
			Config::UuidBlacklist = UuidBlacklist->GetData();
			UuidBlacklist->Release();
		}

		_LOAD(Int32, UseCache, 1);
		_LOAD(Int32, CacheDepth, 2);
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