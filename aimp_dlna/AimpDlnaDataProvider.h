#pragma once

class AimpDlnaDataProvider : public IUnknownInterfaceImpl<IAIMPMLDataProvider> {
private:
	PLT_SyncMediaBrowser* mediaBrowser;

	int RecursiveBrowse(PLT_DeviceDataReference& device, const string& currentContainer, PLT_MediaObjectListReference& result, const int depth = 0);

public:
	AimpDlnaDataProvider(PLT_SyncMediaBrowser* mediaBrowser) {
		this->mediaBrowser = mediaBrowser;

		AddRef();
	}

	~AimpDlnaDataProvider() {}

	HRESULT WINAPI GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data);

	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject);
};