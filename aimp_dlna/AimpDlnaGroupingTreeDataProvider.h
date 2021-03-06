#pragma once

class AimpDlnaGroupingTreeDataProvider : public IUnknownInterfaceImpl<IAIMPMLGroupingTreeDataProvider, IID_IAIMPMLGroupingTreeDataProvider> {
private:
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

	HRESULT GetRootData(IAIMPMLGroupingTreeDataProviderSelection** Data);
	HRESULT GetChildrenData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data);

public:
	AimpDlnaGroupingTreeDataProvider(shared_ptr<PLT_SyncMediaBrowser> mediaBrowser) {
		this->mediaBrowser = mediaBrowser;
	}

	HRESULT WINAPI AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection);
	DWORD WINAPI GetCapabilities();
	HRESULT WINAPI GetData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data);
	HRESULT WINAPI GetFieldForAlphabeticIndex(IAIMPString** FieldName);
};