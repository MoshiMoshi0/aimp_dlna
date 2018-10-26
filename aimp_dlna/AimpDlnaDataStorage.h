#pragma once

#include "AimpDlnaDataProvider.h"
#include "AimpDlnaGroupingTreeDataProvider.h"

class AimpDlnaDataStorage : public IUnknownInterfaceImpl<IAIMPMLExtensionDataStorage, IID_IAIMPMLExtensionDataStorage> {
private:
	static AimpDlnaDataStorage* singleton;

	bool finalized{ false };
	IAIMPMLDataStorageManager* manager;
	IAIMPMLDataProvider* dataProvider;

	PLT_UPnP* upnp;
	PLT_CtrlPoint* ctrlPoint;
	PLT_SyncMediaBrowser* mediaBrowser;
	PLT_CtrlPointReference ctrlPointRef;

	AimpDlnaDataStorage() : manager(nullptr), dataProvider(nullptr), mediaBrowser(nullptr) {
		upnp = new PLT_UPnP();
		ctrlPoint = new PLT_CtrlPoint();
		ctrlPointRef = PLT_CtrlPointReference(ctrlPoint);
		mediaBrowser = new PLT_SyncMediaBrowser(ctrlPointRef, Config::UseCache);

		dataProvider = new AimpDlnaDataProvider(mediaBrowser);
	}

	class DataStorageManagerRefreshTask : public NPT_Thread {
	private:
		IAIMPMLDataStorageManager* manager;
		PLT_SyncMediaBrowser* mediaBrowser;

		DataStorageManagerRefreshTask(IAIMPMLDataStorageManager* Manager, PLT_SyncMediaBrowser* MediaBrowser) {
			manager = Manager;
			manager->AddRef();

			mediaBrowser = MediaBrowser;
		}
	public:
		static NPT_Result Start(IAIMPMLDataStorageManager* Manager, PLT_SyncMediaBrowser* MediaBrowser) {
			auto instance = new DataStorageManagerRefreshTask(Manager, MediaBrowser);
			return static_cast<NPT_ThreadInterface*>(instance)->Start();
		}

		~DataStorageManagerRefreshTask() {
			manager->Release();
			manager = nullptr;
			mediaBrowser = nullptr;
		}

		void Run() {
			auto count = 0u;
			auto timeout = Config::ScanDuration / 1000.0;
			auto updateRate = !Config::ScanStop ? timeout : 0.1;
			while(timeout > 0) {
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

			delete this;
		}
	};
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

