#include "stdafx.h"
#include "AimpDlnaDataProvider.h"
#include "AimpDlnaDataProviderSelection.h"

HRESULT WINAPI AimpDlnaDataProvider::GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data) { 
	vector<wstring> breadcrumbs;

	IAIMPMLDataFieldFilter* childFilter = nullptr;
	if (SUCCEEDED(Filter->GetChild(0, IID_IAIMPMLDataFieldFilter, reinterpret_cast<void**>(&childFilter)))) {
		IAIMPString* value1 = nullptr;
		IAIMPString* value2 = nullptr;
		if (SUCCEEDED(childFilter->GetValueAsObject(AIMPML_FIELDFILTER_VALUE1, IID_IAIMPString, reinterpret_cast<void**>(&value1)))) {
			wstring value = wstring(value1->GetData());
			breadcrumbs.push_back(value);
			value1->Release();
		}		
		if (SUCCEEDED(childFilter->GetValueAsObject(AIMPML_FIELDFILTER_VALUE2, IID_IAIMPString, reinterpret_cast<void**>(&value2)))) {
			wstring value = wstring(value2->GetData());
			if (value.length() > 0) {
				breadcrumbs.push_back(value);
			}
			value2->Release();
		}
		childFilter->Release();
	}

	if (breadcrumbs.size() == 0)
		return E_FAIL;

	auto deviceUuid = StringUtils::ToString(breadcrumbs.back());
	auto containerId = breadcrumbs.size() == 1 ? "0" : StringUtils::ToString(breadcrumbs.front());

	PLT_DeviceDataReference device;
	if (NPT_FAILED(mediaBrowser->FindServer(deviceUuid.c_str(), device)))
		return E_FAIL;

	PLT_MediaObjectListReference objects(new PLT_MediaObjectList());
	if (NPT_FAILED(RecursiveBrowse(device, containerId, objects)))
		return E_FAIL;

	if (objects.IsNull() || objects->GetItemCount() == 0)
		return E_FAIL;

	*Data = new AimpDlnaDataProviderSelection(deviceUuid, containerId, objects, AimpUtils::ToWideStringList(Fields));
	return S_OK;
}

int AimpDlnaDataProvider::RecursiveBrowse(PLT_DeviceDataReference& device, const string& currentContainer, PLT_MediaObjectListReference& result, const int depth) {
	PLT_MediaObjectListReference objects;
	if (NPT_FAILED(mediaBrowser->BrowseSync(device, currentContainer.c_str(), objects)))
		return NPT_FAILURE;

	if (objects.IsNull())
		return NPT_SUCCESS;

	for (auto object = objects->GetFirstItem(); object; object++) {
		if (!(*object)->IsContainer()) {
			result->Add(*object);
		} else if (depth < Config::CacheDepth) {
			if (mediaBrowser->IsCached(device->GetUUID(), (*object)->m_ObjectID.GetChars()))
				if (NPT_FAILED(RecursiveBrowse(device, StringUtils::ToString((*object)->m_ObjectID), result, depth + 1)))
					return NPT_FAILURE;
		}
	}

	return NPT_SUCCESS;
}