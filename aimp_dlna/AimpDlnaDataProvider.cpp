#include "stdafx.h"
#include "AimpDlnaDataProvider.h"
#include "AimpDlnaDataProviderSelection.h"

HRESULT WINAPI AimpDlnaDataProvider::GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data) { 
	vector<wstring> breadcrumbs;

	for (size_t i = 0; i < Filter->GetChildCount(); i++) {
		IAIMPMLDataFieldFilter* childFilter = nullptr;
		if (SUCCEEDED(Filter->GetChild(i, IID_IAIMPMLDataFieldFilter, reinterpret_cast<void**>(&childFilter)))) {
			IAIMPString* value = nullptr;
			if (SUCCEEDED(childFilter->GetValueAsObject(AIMPML_FIELDFILTER_VALUE1, IID_IAIMPString, reinterpret_cast<void**>(&value)))) {
				breadcrumbs.push_back(wstring(value->GetData()));
				value->Release();
			}
			childFilter->Release();
		}
	}

	if (breadcrumbs.size() == 0)
		return E_FAIL;

	auto deviceUuid = StringUtils::ToString(breadcrumbs.back());
	auto containerId = breadcrumbs.size() == 1 ? "0" : StringUtils::ToString(breadcrumbs.front());

	PLT_DeviceDataReference device;
	if (NPT_FAILED(mediaBrowser->FindServer(deviceUuid.c_str(), device)))
		return E_FAIL;

	const auto recursive_browse = [&](const auto& self, const char* currentContainer, PLT_MediaObjectListReference& result, const int depth = 0) -> int {
		if (depth >= 3)
			return NPT_SUCCESS;

		PLT_MediaObjectListReference objects;
		if (NPT_FAILED(mediaBrowser->BrowseSync(device, currentContainer, objects, false)))
			return NPT_FAILURE;

		if (objects.IsNull())
			return NPT_SUCCESS;

		for (auto object = objects->GetFirstItem(); object; object++) {
			if (!(*object)->IsContainer()) {
				result->Add(*object);
			} else {
				if (mediaBrowser->IsCached(device->GetUUID(), (*object)->m_ObjectID.GetChars()))
					if (NPT_FAILED(self(self, (*object)->m_ObjectID.GetChars(), result, depth + 1)))
						return NPT_FAILURE;
			}
		}

		return NPT_SUCCESS;
	};

	PLT_MediaObjectListReference objects(new PLT_MediaObjectList());
	if (NPT_FAILED(recursive_browse(recursive_browse, containerId.c_str(), objects)))
		return E_FAIL;

	if (objects.IsNull() || objects->GetItemCount() == 0)
		return E_FAIL;

	vector<wstring> fields;
	for (size_t i = 0; i < Fields->GetCount(); i++) {
		IAIMPString* value = nullptr;
		if (SUCCEEDED(Fields->GetObject(i, IID_IAIMPString, reinterpret_cast<void**>(&value)))) {
			fields.push_back(wstring(value->GetData()));
			value->Release();
		}
	}

	*Data = new AimpDlnaDataProviderSelection(objects, fields);
	return S_OK;
}

HRESULT WINAPI AimpDlnaDataProvider::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLDataProvider) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}