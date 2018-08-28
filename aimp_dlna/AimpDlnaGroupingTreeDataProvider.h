#pragma once

class AimpDlnaGroupingTreeDataProvider : public IUnknownInterfaceImpl<IAIMPMLGroupingTreeDataProvider> {
public:
	AimpDlnaGroupingTreeDataProvider(PLT_SyncMediaBrowser* mediaBrowser) {
		AddRef();
	}

	HRESULT WINAPI AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection);
	DWORD WINAPI GetCapabilities();
	HRESULT WINAPI GetData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data);
	HRESULT WINAPI GetFieldForAlphabeticIndex(IAIMPString** FieldName);
	
	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject);
};