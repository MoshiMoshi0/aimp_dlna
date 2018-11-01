#pragma once

class AimpDlnaDataProvider : public IUnknownInterfaceImpl<IAIMPMLDataProvider, IID_IAIMPMLDataProvider> {
private:
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

	int RecursiveBrowse(PLT_DeviceDataReference& device, const string& currentContainer, PLT_MediaObjectListReference& result, const int depth = 0);

public:
	AimpDlnaDataProvider(shared_ptr<PLT_SyncMediaBrowser> mediaBrowser) {
		this->mediaBrowser = mediaBrowser;
	}

	~AimpDlnaDataProvider() {}

	HRESULT WINAPI GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data);
};