#pragma once

class AimpDlnaGroupingTreeDataProvider : public IUnknownInterfaceImpl<IAIMPMLGroupingTreeDataProvider> {
public:
	HRESULT WINAPI AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection);
	DWORD WINAPI GetCapabilities();
	HRESULT WINAPI GetData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data);
	HRESULT WINAPI GetFieldForAlphabeticIndex(IAIMPString** FieldName);
};