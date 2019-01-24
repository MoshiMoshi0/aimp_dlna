#pragma once

#include "AimpDlnaDataProvider.h"
#include "AimpDlnaAlbumArtProvider.h"
#include "AimpDlnaGroupingTreeDataProvider.h"

class AimpDlnaDataStorage : public IUnknownInterfaceImpl<IAIMPMLExtensionDataStorage, IID_IAIMPMLExtensionDataStorage> {
private:
	bool finalized{ false };
	IAIMPMLDataStorageManager* manager;	
	IAIMPMLDataProvider* dataProvider;
	IAIMPMLAlbumArtProvider* albumArtProvider;

	shared_ptr<PLT_UPnP> upnp;
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

	PLT_CtrlPointReference ctrlPoint;
	PLT_TaskManagerReference taskManager;

public:
	AimpDlnaDataStorage() : manager(nullptr), dataProvider(nullptr), mediaBrowser(nullptr) {
		ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
		taskManager = PLT_TaskManagerReference(new PLT_TaskManager());

		upnp = make_shared<PLT_UPnP>();
		mediaBrowser = make_shared<PLT_SyncMediaBrowser>(ctrlPoint, Config::UseCache);

		dataProvider = new AimpDlnaDataProvider(mediaBrowser);
		albumArtProvider = new AimpDlnaAlbumArtProvider(mediaBrowser);
	}

	~AimpDlnaDataStorage() {
		if (!finalized)
			Finalize();
	}

	void WINAPI Initialize(IAIMPMLDataStorageManager* Manager);
	void WINAPI Finalize();

	void WINAPI FlushCache(int Reserved);
	HRESULT WINAPI ConfigLoad(IAIMPConfig* Config, IAIMPString* Section);
	HRESULT WINAPI ConfigSave(IAIMPConfig* Config, IAIMPString* Section);

	HRESULT WINAPI GetFields(int Schema, IAIMPObjectList** List);
	HRESULT WINAPI GetGroupingPresets(int Schema, IAIMPMLGroupingPresets* Presets);

	void WINAPI BeginUpdate() {}
	void WINAPI EndUpdate() {}
	HRESULT WINAPI Reset() { return S_OK; }

	HRESULT WINAPI GetValueAsFloat(int PropertyID, double *Value) { return S_OK; }
	HRESULT WINAPI GetValueAsInt32(int PropertyID, int *Value);
	HRESULT WINAPI GetValueAsInt64(int PropertyID, INT64 *Value) { return S_OK; }
	HRESULT WINAPI GetValueAsObject(int PropertyID, REFIID IID, void **Value);

	HRESULT WINAPI SetValueAsFloat(int PropertyID, const double Value) { return S_OK; }
	HRESULT WINAPI SetValueAsInt32(int PropertyID, int Value) { return S_OK; }
	HRESULT WINAPI SetValueAsInt64(int PropertyID, const INT64 Value) { return S_OK; }
	HRESULT WINAPI SetValueAsObject(int PropertyID, IUnknown *Value) { return S_OK; }

	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject);
};

class DataStorageManagerRefreshTask : public PLT_ThreadTask {
private:
	IAIMPMLDataStorageManager* manager;
	
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

public:
	DataStorageManagerRefreshTask(IAIMPMLDataStorageManager* Manager, shared_ptr<PLT_SyncMediaBrowser> MediaBrowser) {
		manager = Manager;
		manager->AddRef();

		mediaBrowser = MediaBrowser;
	}

protected:
	virtual ~DataStorageManagerRefreshTask() {
		if (manager != nullptr) {
			manager->Release();
			manager = nullptr;
		}

		mediaBrowser = nullptr;
	}

	virtual void DoRun() {
		auto count = 0u;
		auto timeout = Config::ScanDuration / 1000.0;
		auto updateRate = !Config::ScanStop ? timeout : 0.1;

		while (timeout > 0 && !IsAborting(0)) {
			NPT_System::Sleep(updateRate);
			timeout -= updateRate;

			auto newCount = mediaBrowser->GetMediaServers().GetItemCount();
			if (Config::ScanStop && newCount > count) {
				timeout = Config::StopDelay / 1000.0;
			}

			count = newCount;
		}

		if (count > 0)
			manager->Changed();
	}
};