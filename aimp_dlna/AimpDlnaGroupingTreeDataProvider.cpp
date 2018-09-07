#include "stdafx.h"
#include "AimpDlnaGroupingTreeDataProvider.h"
#include "AimpDlnaGroupingTreeDataProviderSelection.h"

using namespace std;

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection) {
	Selection->AddRef();
	Filter->AddRef();
	Filter->BeginUpdate();
	Filter->SetValueAsInt32(AIMPML_FILTERGROUP_OPERATION, AIMPML_FILTERGROUP_OPERATION_AND);
	for (size_t i = 0; i < (size_t)Selection->GetCount(); i++) {
		IAIMPString* fieldName = nullptr;
		VARIANT value;

		if (SUCCEEDED(Selection->GetValue(i, &fieldName, &value))) {
			IAIMPMLDataFieldFilter* outFilter = nullptr;
			auto result = Filter->Add(fieldName, &value, &VARIANT(), AIMPML_FIELDFILTER_OPERATION_EQUALS, &outFilter);

			fieldName->Release();
			if (FAILED(result)) {
				return E_FAIL;
			}

			outFilter->Release();
		}
	}
	Filter->EndUpdate();
	Filter->Release();
	Selection->Release();
	return S_OK;
}

DWORD WINAPI AimpDlnaGroupingTreeDataProvider::GetCapabilities() {
	return AIMPML_GROUPINGTREEDATAPROVIDER_CAP_HIDEALLDATA | AIMPML_GROUPINGTREEDATAPROVIDER_CAP_DONTSORT;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::GetData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data) {
	Selection->AddRef();

	auto result = S_OK;
	if (Selection->GetCount() == 0) {
		result = GetRootData(Data);
	} else {
		result = GetChildrenData(Selection, Data);
	}

	Selection->Release();
	return result;
}

HRESULT AimpDlnaGroupingTreeDataProvider::GetRootData(IAIMPMLGroupingTreeDataProviderSelection** Data){
	const auto& devices = mediaBrowser->GetMediaServers();
	NPT_AutoLock lock((NPT_Mutex&)devices);

	auto list = vector<AimpDlnaGroupingTreeDataProviderNode>();
	for (auto device = devices.GetFirstItem(); device; device++) {
		auto displayName = StringUtils::ToWideString((*device)->GetFriendlyName());
		auto value = StringUtils::ToWideString((*device)->GetUUID());
		AimpDlnaGroupingTreeDataProviderNode node = { AIMPML_FIELDIMAGE_NOTE, value, displayName, true, true };
		list.push_back(node);
	}

	if (list.size() == 0)
		return E_FAIL;
	
	*Data = new AimpDlnaGroupingTreeDataProviderSelection(list);
	return S_OK;
}

HRESULT AimpDlnaGroupingTreeDataProvider::GetChildrenData(IAIMPMLGroupingTreeSelection* Selection, IAIMPMLGroupingTreeDataProviderSelection** Data) {
	vector<wstring> breadcrumbs;

	for (size_t i = 0; i < (size_t)Selection->GetCount(); i++) {
		IAIMPString* fieldName = nullptr;
		VARIANT value;

		if (SUCCEEDED(Selection->GetValue(i, &fieldName, &value))) {
			breadcrumbs.push_back(wstring(value.bstrVal));
			fieldName->Release();
		}
	}

	if (breadcrumbs.size() == 0)
		return E_FAIL;

	auto deviceUuid = StringUtils::ToString(breadcrumbs.back());
	auto containerId = breadcrumbs.size() == 1 ? "0" : StringUtils::ToString(breadcrumbs.front());

	PLT_DeviceDataReference device;
	if (NPT_FAILED(mediaBrowser->FindServer(deviceUuid.c_str(), device)))
		return E_FAIL;

	PLT_MediaObjectListReference objects;
	if (NPT_FAILED(mediaBrowser->BrowseSync(device, containerId.c_str(), objects, false, 0, UINT_MAX)))
		return E_FAIL;

	if (objects.IsNull())
		return E_FAIL;

	auto list = vector<AimpDlnaGroupingTreeDataProviderNode>();
	for (auto object = objects->GetFirstItem(); object; object++) {
		if (!(*object)->IsContainer())
			continue;

		auto displayName = StringUtils::ToWideString((*object)->m_Title);
		auto value = StringUtils::ToWideString((*object)->m_ObjectID);
		list.push_back({ AIMPML_FIELDIMAGE_FOLDER , value, displayName, false, true });
	}

	if (list.size() == 0)
		return E_FAIL;

	*Data = new AimpDlnaGroupingTreeDataProviderSelection(list);
	return S_OK;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::GetFieldForAlphabeticIndex(IAIMPString** FieldName) { return E_FAIL; }

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLGroupingTreeDataProvider) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}