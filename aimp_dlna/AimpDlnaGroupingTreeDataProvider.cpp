#include "stdafx.h"
#include "AimpDlnaGroupingTreeDataProvider.h"
#include "AimpDlnaGroupingTreeDataProviderSelection.h"

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection) {
	Filter->BeginUpdate();
	Filter->SetValueAsInt32(AIMPML_FILTERGROUP_OPERATION, AIMPML_FILTERGROUP_OPERATION_AND);
	for (size_t i = 0; i < Selection->GetCount(); i++) {
		IAIMPString* fieldName = nullptr;
		VARIANT value;

		if (SUCCEEDED(Selection->GetValue(i, &fieldName, &value))) {
			IAIMPMLDataFieldFilter* outFilter = nullptr;
			if (!Filter->Add(fieldName, &value, &VARIANT(), AIMPML_FIELDFILTER_OPERATION_EQUALS, &outFilter)) {
				return E_FAIL;
			}
		}
	}
	Filter->EndUpdate();
	return S_OK;
}

DWORD WINAPI AimpDlnaGroupingTreeDataProvider::GetCapabilities() {
	return AIMPML_GROUPINGTREEDATAPROVIDER_CAP_HIDEALLDATA | AIMPML_GROUPINGTREEDATAPROVIDER_CAP_DONTSORT;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::GetData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data) {
	if (Selection->GetCount() == 0) {
		auto list = vector<AimpDlnaGroupingTreeDataProviderNode>();
		AimpDlnaGroupingTreeDataProviderNode node;
		node.ImageIndex = AIMPML_FIELDIMAGE_NOTE;
		node.Value = L"1";
		node.DisplayTitle = L"1";
		node.Standalone = true;
		node.HasChildren = false;
		list.push_back(node);
		node.Value = L"2";
		node.DisplayTitle = L"2";
		list.push_back(node);
		node.Value = L"3";
		node.DisplayTitle = L"3";
		list.push_back(node);
		*Data = new AimpDlnaGroupingTreeDataProviderSelection(list);
		return S_OK;
	} else {
	}
	return E_FAIL;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::GetFieldForAlphabeticIndex(IAIMPString** FieldName) { return E_FAIL; }