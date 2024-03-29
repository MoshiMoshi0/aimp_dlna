#include "stdafx.h"
#include "AimpDlnaGroupingTreeDataProvider.h"
#include "AimpDlnaGroupingTreeDataProviderSelection.h"

using namespace std;

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::AppendFilter(IAIMPMLDataFilterGroup* Filter, IAIMPMLGroupingTreeSelection* Selection) {
	Selection->AddRef();
	Filter->AddRef();
	Filter->BeginUpdate();
	Filter->SetValueAsInt32(AIMPML_FILTERGROUP_OPERATION, AIMPML_FILTERGROUP_OPERATION_AND);

	auto result = S_OK;
	try {
		if (Selection->GetCount() == 0)
			throw exception();

		IAIMPString* fieldName = nullptr;
		VARIANT value1 = VARIANT();
		VARIANT value2 = VARIANT();

		if (FAILED(Selection->GetValue(0, &fieldName, &value1)))
			throw exception();

		if (Selection->GetCount() > 1) {
			fieldName->Release();
			fieldName = nullptr;

			if (FAILED(Selection->GetValue(Selection->GetCount() - 1, &fieldName, &value2)))
				throw exception();
		}

		IAIMPMLDataFieldFilter* outFilter = nullptr;
		if (SUCCEEDED(Filter->Add(fieldName, &value1, &value2, AIMPML_FIELDFILTER_OPERATION_EQUALS, &outFilter))) {
			outFilter->Release();
			fieldName->Release();
		} else {
			throw exception();
		}
	} catch (const exception&) {
		result = E_FAIL;
	}

	Filter->EndUpdate();
	Filter->Release();
	Selection->Release();

	if (FAILED(result))
		return E_FAIL;
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

		list.push_back({ AIMPML_FIELDIMAGE_NOTE, value, displayName, true, true });
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
		VARIANT value = VARIANT();

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
	if (NPT_FAILED(mediaBrowser->BrowseSync(device, containerId.c_str(), objects)))
		return E_FAIL;

	if (objects.IsNull())
		return E_FAIL;

	auto list = vector<AimpDlnaGroupingTreeDataProviderNode>();
	for (auto object = objects->GetFirstItem(); object; object++) {
		if (!(*object)->IsContainer())
			continue;

		auto displayName = StringUtils::ToWideString((*object)->m_Title);
		auto value = StringUtils::ToWideString((*object)->m_ObjectID);
		auto childrenCount = reinterpret_cast<PLT_MediaContainer*>(*object)->m_ChildrenCount;
		auto hasChildren = childrenCount == -1 || childrenCount > 0;
		
		list.push_back({ AIMPML_FIELDIMAGE_FOLDER , value, displayName, false,  hasChildren });
	}

	if (list.size() == 0)
		return E_FAIL;

	*Data = new AimpDlnaGroupingTreeDataProviderSelection(list);
	return S_OK;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProvider::GetFieldForAlphabeticIndex(IAIMPString** FieldName) { return E_FAIL; }