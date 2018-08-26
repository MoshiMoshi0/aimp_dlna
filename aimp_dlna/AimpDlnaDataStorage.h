#pragma once

#include "AimpDlnaDataProvider.h"
#include "AimpDlnaGroupingTreeDataProvider.h"

class AimpDlnaDataStorage : public IUnknownInterfaceImpl<IAIMPMLExtensionDataStorage> {
private:
	static AimpDlnaDataStorage* singleton;

	const wstring id{ L"AimpDlna" };
	const wstring caption{ L"DLNA" };
	const int capabilities{ 0 };

	bool finalized{ false };
	IAIMPMLDataStorageManager* manager;
	IAIMPMLDataProvider* dataProvider;
	IAIMPMLGroupingTreeDataProvider* groupingTreeDataProvider;

	AimpDlnaDataStorage() : manager(nullptr) {
		AddRef();

		dataProvider = new AimpDlnaDataProvider();
		groupingTreeDataProvider = new AimpDlnaGroupingTreeDataProvider();
	}
public:
	static AimpDlnaDataStorage* instance() {
		if (!singleton)
			singleton = new AimpDlnaDataStorage();

		return singleton;
	}

	~AimpDlnaDataStorage() {
		if (!finalized)
			Finalize();

		singleton = nullptr;
	}

	//IAIMPMLExtensionDataStorage
	void WINAPI Initialize(IAIMPMLDataStorageManager* Manager);
	void WINAPI Finalize();

	void WINAPI FlushCache(int Reserved);
	HRESULT WINAPI ConfigLoad(IAIMPConfig* Config, IAIMPString* Section);
	HRESULT WINAPI ConfigSave(IAIMPConfig* Config, IAIMPString* Section);

	HRESULT WINAPI GetFields(int Schema, IAIMPObjectList** List);
	HRESULT WINAPI GetGroupingPresets(int Schema, IAIMPMLGroupingPresets* Presets);

	//IAIMPPropertyList
	void WINAPI BeginUpdate();
	void WINAPI EndUpdate();
	HRESULT WINAPI Reset();

	HRESULT WINAPI GetValueAsFloat(int PropertyID, double *Value) { return S_OK; }
	HRESULT WINAPI GetValueAsInt32(int PropertyID, int *Value);
	HRESULT WINAPI GetValueAsInt64(int PropertyID, INT64 *Value) { return S_OK; }
	HRESULT WINAPI GetValueAsObject(int PropertyID, REFIID IID, void **Value);

	HRESULT WINAPI SetValueAsFloat(int PropertyID, const double Value) { return S_OK; }
	HRESULT WINAPI SetValueAsInt32(int PropertyID, int Value) { return S_OK; }
	HRESULT WINAPI SetValueAsInt64(int PropertyID, const INT64 Value) { return S_OK; }
	HRESULT WINAPI SetValueAsObject(int PropertyID, IUnknown *Value) { return S_OK; }

	//IUnknown
	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject);
};

