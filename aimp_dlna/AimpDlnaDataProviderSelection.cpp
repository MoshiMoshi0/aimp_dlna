#include "stdafx.h"
#include "AimpDlnaDataProviderSelection.h"
#include "AimpDlnaMusicLibrary.h"

PLT_MediaObject* AimpDlnaDataProviderSelection::GetItem(unsigned int index) {
	if (index >= items->GetItemCount())
		return nullptr;

	PLT_MediaObject* item;
	items->Get(index, item);
	return item;
}

wstring AimpDlnaDataProviderSelection::GetField(unsigned int index) {
	if (index >= fields.size())
		return L"";

	return fields[index];
}

DOUBLE WINAPI AimpDlnaDataProviderSelection::GetValueAsFloat(int FieldIndex) {
	auto item = GetItem(index);
	auto field = GetField(FieldIndex);
	if (item == nullptr)
		return DOUBLE();
	if (item->m_Resources.GetItemCount() == 0)
		return DOUBLE();

	auto resource = item->m_Resources[0];
	if (field.compare(EVDS_TrackDuration) == 0) {
		return (DOUBLE)resource.m_Duration;
	} else if (field.compare(EVDS_TrackDate) == 0) {
		for (int format = 0; format <= NPT_DateTime::FORMAT_RFC_1036; format++) {
			NPT_DateTime date;
			NPT_TimeStamp timestamp;
			if (NPT_SUCCEEDED(date.FromString(item->m_Date, (NPT_DateTime::Format)format)) &&
				NPT_SUCCEEDED(date.ToTimeStamp(timestamp))) {
				return (DOUBLE)(25569.0 + timestamp.ToSeconds() / 86400.0);
			}
		}

		return DOUBLE();
	}

	return DOUBLE();
}

int WINAPI AimpDlnaDataProviderSelection::GetValueAsInt32(int FieldIndex) {
	auto item = GetItem(index);
	auto field = GetField(FieldIndex);
	if (item == nullptr)
		return int();

	if (field.compare(EVDS_TrackNumber) == 0) {
		auto trackNumber = item->m_MiscInfo.original_track_number;
		return trackNumber <= 0 ? index : trackNumber;
	}

	return int();
}

WCHAR* WINAPI AimpDlnaDataProviderSelection::GetValueAsString(int FieldIndex, int* Length) {
	auto item = GetItem(index);
	auto field = GetField(FieldIndex);

	*Length = 0;
	if (item == nullptr)
		return nullptr;

	NPT_String result;
	if (field.compare(EVDS_TrackFileName) == 0) {
		if (item->m_Resources.GetItemCount() == 0)
			return nullptr;

		auto resource = item->m_Resources[0];
		result = resource.m_Uri;
	} else if (field.compare(EVDS_NodeId) == 0) {
		result = item->m_ObjectID;
	} else if (field.compare(EVDS_TrackId) == 0) {
		result = item->m_ParentID + ":" + item->m_ObjectID;
	} else if (field.compare(EVDS_TrackArtist) == 0) {
		PLT_StringList artists;
		for (auto person = item->m_People.artists.GetFirstItem(); person; person++) {
			artists.Add((*person).name);
		}
		result = NPT_String::Join(artists, ", ");
	} else if (field.compare(EVDS_TrackAlbum) == 0) {
		result = item->m_Affiliation.album;
	} else if (field.compare(EVDS_TrackTitle) == 0) {
		result = item->m_Title;
	}

	*Length = result.GetLength();
	return StringUtils::ToWideCharArray(result);
}

BOOL WINAPI AimpDlnaDataProviderSelection::NextRow() {
	if (items->GetItemCount() == 0)
		return false;

	auto result = ++index < items->GetItemCount();
	if (!result)
		index = 0;

	return result;
}

HRESULT WINAPI AimpDlnaDataProviderSelection::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLDataProviderSelection) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}