#include "stdafx.h"
#include "AimpDlnaDataProvider.h"
#include "AimpGenericDataProviderSelection.h"
#include "AimpDlnaMusicLibrary.h"

HRESULT WINAPI AimpDlnaDataProvider::GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data) { 
	vector<wstring> breadcrumbs;

	for (size_t i = 0; i < Filter->GetChildCount(); i++) {
		IAIMPMLDataFieldFilter* childFilter = nullptr;
		if (SUCCEEDED(Filter->GetChild(i, IID_IAIMPMLDataFieldFilter, reinterpret_cast<void**>(&childFilter)))) {
			IAIMPString* value = nullptr;
			if (SUCCEEDED(childFilter->GetValueAsObject(AIMPML_FIELDFILTER_VALUE1, IID_IAIMPString, reinterpret_cast<void**>(&value)))) {
				breadcrumbs.push_back(wstring(value->GetData()));
			}				
		}
	}

	if (breadcrumbs.size() == 0)
		return E_FAIL;

	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	auto deviceUuid = converter.to_bytes(breadcrumbs.back());
	auto containerId = breadcrumbs.size() == 1 ? "0" : converter.to_bytes(breadcrumbs.front());

	PLT_DeviceDataReference device;
	if (FAILED(mediaBrowser->FindServer(deviceUuid.c_str(), device)))
		return E_FAIL;

	PLT_MediaObjectListReference objects;
	if (FAILED(mediaBrowser->BrowseSync(device, containerId.c_str(), objects, false, 0, UINT_MAX)))
		return E_FAIL;

	vector<wstring> fields;
	for (size_t i = 0; i < Fields->GetCount(); i++) {
		IAIMPString* value = nullptr;
		if (SUCCEEDED(Fields->GetObject(i, IID_IAIMPString, reinterpret_cast<void**>(&value)))) {
			fields.push_back(wstring(value->GetData()));
		}
	}

	auto list = vector<PLT_MediaItem>();
	for (auto object = objects->GetFirstItem(); object; object++) {
		if ((*object)->IsContainer())
			continue;

		list.push_back(*reinterpret_cast<PLT_MediaItem*>(*object));
	}

	if (list.size() == 0)
		return E_FAIL;

	AimpGenericDataProviderSelectionSelectors<PLT_MediaItem> selectors = {
		[](PLT_MediaItem* item, wstring field, int index) -> DOUBLE {
			if (item == nullptr)
				return DOUBLE();
			if (item->m_Resources.GetItemCount() == 0)
				return DOUBLE();

			auto resource = item->m_Resources[0];
			if (field.compare(EVDS_TrackDuration) == 0)
				return (DOUBLE)resource.m_Duration;

			return DOUBLE();
		},
		[](PLT_MediaItem* item, wstring field, int index) -> int {
			if (item == nullptr)
				return int();

			if (field.compare(EVDS_TrackNumber) == 0)
				return (int)item->m_MiscInfo.original_track_number;

			return int();
		},
		[](PLT_MediaItem* item, wstring field, int index) -> INT64 { return INT64(); },
		[](PLT_MediaItem* item, wstring field, int index, int* length) -> WCHAR* {
			*length = 0;
			if (item == nullptr)
				return nullptr;

			NPT_String result;
			if (field.compare(EVDS_TrackFileName) == 0) {
				if (item->m_Resources.GetItemCount() == 0)
					return nullptr;

				auto resource = item->m_Resources[0];
				result = resource.m_Uri;
			} else if (field.compare(EVDS_TrackId) == 0 || field.compare(EVDS_NodeId) == 0) {
				result = item->m_ObjectID;
			} else if (field.compare(EVDS_TrackArtist) == 0) {
				PLT_StringList artists;
				for (auto person = item->m_People.artists.GetFirstItem(); person; person++) {
					artists.Add((*person).name);
				}
				result = NPT_String::Join(artists, ", ");
			} else if (field.compare(EVDS_TrackDate) == 0) {
				result = item->m_Date;
			} else if (field.compare(EVDS_TrackAlbum) == 0) {
				result = item->m_Affiliation.album;
			} else if (field.compare(EVDS_TrackTitle) == 0) {
				result = item->m_Title;
			}

			wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
			wstring wresult = converter.from_bytes(result.GetChars());
			*length = wresult.length();
			return (WCHAR*)wresult.c_str();
		}
	};

	*Data = new AimpGenericDataProviderSelection<PLT_MediaItem>(list, fields, selectors);

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