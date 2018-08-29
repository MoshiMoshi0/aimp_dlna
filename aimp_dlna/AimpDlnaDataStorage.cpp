#include "stdafx.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaMusicLibrary.h"

AimpDlnaDataStorage* AimpDlnaDataStorage::singleton = nullptr;

void AimpDlnaDataStorage::Initialize(IAIMPMLDataStorageManager* Manager) {
	upnp->AddCtrlPoint(ctrlPointRef);
	upnp->Start();

	ctrlPoint->Discover();
}

void AimpDlnaDataStorage::Finalize() {
	if (upnp != nullptr) {
		if (upnp->IsRunning())
			upnp->Stop();
		delete upnp;
	}

	ctrlPointRef.Detach();
	if (ctrlPoint != nullptr) {
		delete ctrlPoint;
	}

	if (dataProvider != nullptr) {
		dataProvider->Release();
		dataProvider = nullptr;
	}
	if (groupingTreeDataProvider != nullptr) {
		groupingTreeDataProvider->Release();
		groupingTreeDataProvider = nullptr;
	}
	if (mediaBrowser != nullptr)
		delete mediaBrowser;
}

void AimpDlnaDataStorage::FlushCache(int Reserved) {}
HRESULT AimpDlnaDataStorage::ConfigLoad(IAIMPConfig* Config, IAIMPString* Section) { return S_OK; }
HRESULT AimpDlnaDataStorage::ConfigSave(IAIMPConfig* Config, IAIMPString* Section) { return S_OK; }

HRESULT AimpDlnaDataStorage::GetFields(int Schema, IAIMPObjectList** List) {
	Plugin::instance()->Core()->CreateObject(IID_IAIMPObjectList, reinterpret_cast<void**>(List));

	switch (Schema) {
	case AIMPML_FIELDS_SCHEMA_ALL: {
		auto addField = [](auto list, wstring name, int type, int flags = 0) {
			IAIMPMLDataField* field = nullptr;
			Plugin::instance()->Core()->CreateObject(IID_IAIMPMLDataField, reinterpret_cast<void**>(&field));

			field->SetValueAsObject(AIMPML_FIELD_PROPID_NAME, AimpString(name));
			field->SetValueAsInt32(AIMPML_FIELD_PROPID_TYPE, type);
			field->SetValueAsInt32(AIMPML_FIELD_PROPID_FLAGS, flags);

			list->Add(field);
		};

		addField(*List, EVDS_TrackId, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_INTERNAL);
		addField(*List, EVDS_TrackFileName, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_INTERNAL);

		addField(*List, EVDS_TrackNumber, AIMPML_FIELDTYPE_INT32);
		addField(*List, EVDS_TrackArtist, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackDate, AIMPML_FIELDTYPE_STRING);
		addField(*List, EVDS_TrackAlbum, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackTitle, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackDuration, AIMPML_FIELDTYPE_DURATION, AIMPML_FIELDFLAG_INTERNAL);

		addField(*List, EVDS_NodeId, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_INTERNAL);
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_ALBUMTHUMBNAILS:
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_GROUPDETAILS:
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_DEFAULT: {
		(*List)->Add(AimpString(EVDS_TrackFileName));
		(*List)->Add(AimpString(EVDS_TrackNumber));
		(*List)->Add(AimpString(EVDS_TrackArtist));
		(*List)->Add(AimpString(EVDS_TrackDate));
		(*List)->Add(AimpString(EVDS_TrackAlbum));
		(*List)->Add(AimpString(EVDS_TrackTitle));
		(*List)->Add(AimpString(EVDS_TrackDuration));
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_GROUPDETAILS: {
		(*List)->Add(AimpString(EVDS_TrackArtist));
		(*List)->Add(AimpString(EVDS_TrackDate));
		(*List)->Add(AimpString(EVDS_TrackAlbum));
		(*List)->Add(AimpString(EVDS_TrackDuration));
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_GROUPBY: {
		(*List)->Add(AimpString(EVDS_TrackArtist));
		(*List)->Add(AimpString(EVDS_TrackAlbum));
		break;
	}
	default:
		break;
	}

	return S_OK;
}

HRESULT WINAPI AimpDlnaDataStorage::GetGroupingPresets(int Schema, IAIMPMLGroupingPresets* Presets) {
	switch (Schema) {
	case AIMPML_GROUPINGPRESETS_SCHEMA_BUILTIN: {
		IAIMPMLGroupingPreset* preset = nullptr;
		Presets->Add(AimpString(id + L".GroupingPreset"), AimpString(L"Default"), 0, groupingTreeDataProvider, &preset);
		break;
	}
	default:
		break;
	}

	return S_OK;
}

//IAIMPPropertyList
void WINAPI AimpDlnaDataStorage::BeginUpdate() {}
void WINAPI AimpDlnaDataStorage::EndUpdate() {}
HRESULT WINAPI AimpDlnaDataStorage::Reset() { return S_OK; }

HRESULT WINAPI AimpDlnaDataStorage::GetValueAsInt32(int PropertyID, int *Value) {
	if (PropertyID == AIMPML_DATASTORAGE_PROPID_CAPABILITIES) {
		*Value = capabilities;
	}

	return S_OK;
}

HRESULT WINAPI AimpDlnaDataStorage::GetValueAsObject(int PropertyID, REFIID IID, void **Value) {
	if (PropertyID == AIMPML_DATASTORAGE_PROPID_ID) {
		*Value = AimpString(id, true);
	}

	if (PropertyID == AIMPML_DATASTORAGE_PROPID_CAPTION) {
		*Value = AimpString(caption, true);
	}

	return S_OK;
}

//IUnknown
HRESULT WINAPI AimpDlnaDataStorage::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLExtensionDataStorage) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	if (riid == IID_IAIMPMLDataProvider) {
		*ppvObject = dataProvider;
		dataProvider->AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}