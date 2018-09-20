#include "stdafx.h"
#include "AimpDlnaDataStorage.h"
#include "AimpDlnaMusicLibrary.h"
#include "AimpDlnaPlugin.h"

AimpDlnaDataStorage* AimpDlnaDataStorage::singleton = nullptr;

void AimpDlnaDataStorage::Initialize(IAIMPMLDataStorageManager* Manager) {
	manager = Manager;
	manager->AddRef();

	upnp->AddCtrlPoint(ctrlPointRef);
	upnp->Start();

	ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "ssdp:all", 5, 0.0, 0.0);

	DataStorageManagerRefreshTask::Start(manager, mediaBrowser);
}

void AimpDlnaDataStorage::Finalize() {
	if (mediaBrowser != nullptr)
		delete mediaBrowser;

	if (upnp != nullptr) {
		if (upnp->IsRunning())
			upnp->Stop();
		delete upnp;
	}

	ctrlPointRef.Detach();
	if (ctrlPoint != nullptr) {
		delete ctrlPoint;
	}

	if (manager != nullptr) {
		manager->Release();
		manager = nullptr;
	}

	if (dataProvider != nullptr) {
		dataProvider->Release();
		dataProvider = nullptr;
	}
}

void AimpDlnaDataStorage::FlushCache(int Reserved) {}
HRESULT AimpDlnaDataStorage::ConfigLoad(IAIMPConfig* Config, IAIMPString* Section) { 
	AimpUtils::Unused(Config);
	AimpUtils::Unused(Section);
	return S_OK;
}

HRESULT AimpDlnaDataStorage::ConfigSave(IAIMPConfig* Config, IAIMPString* Section) {
	AimpUtils::Unused(Config);
	AimpUtils::Unused(Section);
	return S_OK;
}

HRESULT AimpDlnaDataStorage::GetFields(int Schema, IAIMPObjectList** List) {
	*List = AimpUtils::CreateObject<IAIMPObjectList>(IID_IAIMPObjectList);
	if ((*List) == nullptr)
		return E_FAIL;

	switch (Schema) {
	case AIMPML_FIELDS_SCHEMA_ALL: {
		auto addField = [](auto list, wstring name, int type, int flags = 0) {
			IAIMPMLDataField* field = AimpUtils::CreateObject<IAIMPMLDataField>(IID_IAIMPMLDataField);
			if (field == nullptr)
				return;

			field->SetValueAsObject(AIMPML_FIELD_PROPID_NAME, AimpString(name));
			field->SetValueAsInt32(AIMPML_FIELD_PROPID_TYPE, type);
			field->SetValueAsInt32(AIMPML_FIELD_PROPID_FLAGS, flags);

			list->Add(field);
			field->Release();
		};

		addField(*List, EVDS_Id, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_INTERNAL);
		addField(*List, EVDS_TrackFileName, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_INTERNAL);

		addField(*List, EVDS_TrackNumber, AIMPML_FIELDTYPE_INT32);
		addField(*List, EVDS_TrackArtist, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackYear, AIMPML_FIELDTYPE_INT32);
		addField(*List, EVDS_TrackAlbum, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackTitle, AIMPML_FIELDTYPE_STRING, AIMPML_FIELDFLAG_FILTERING);
		addField(*List, EVDS_TrackDuration, AIMPML_FIELDTYPE_DURATION);

		addField(*List, EVDS_NodeId, AIMPML_FIELDTYPE_FILENAME, AIMPML_FIELDFLAG_INTERNAL | AIMPML_FIELDFLAG_REQUIRED);
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_ALBUMTHUMBNAILS:
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_GROUPDETAILS: {
		(*List)->Add(AimpString(EVDS_TrackNumber));
		(*List)->Add(AimpString(EVDS_TrackTitle));
		(*List)->Add(AimpString(EVDS_TrackDuration));
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_VIEW_DEFAULT: {
		(*List)->Add(AimpString(EVDS_TrackNumber));
		(*List)->Add(AimpString(EVDS_TrackArtist));
		(*List)->Add(AimpString(EVDS_TrackYear));
		(*List)->Add(AimpString(EVDS_TrackAlbum));
		(*List)->Add(AimpString(EVDS_TrackTitle));
		(*List)->Add(AimpString(EVDS_TrackDuration));
		break;
	}
	case AIMPML_FIELDS_SCHEMA_TABLE_GROUPDETAILS: {
		(*List)->Add(AimpString(EVDS_TrackArtist));
		(*List)->Add(AimpString(EVDS_TrackAlbum));
		(*List)->Add(AimpString(EVDS_TrackDuration));
		(*List)->Add(AimpString(EVDS_TrackYear));
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
	Presets->AddRef();

	auto result = E_FAIL;
	switch (Schema) {
	case AIMPML_GROUPINGPRESETS_SCHEMA_BUILTIN: {
		const auto presetId = L"AIMP." + id + L".ML.Default";
		IAIMPMLGroupingPreset* preset = nullptr;
		if (SUCCEEDED(Presets->Add(AimpString(presetId), AimpString(Plugin::Lang(L"Common\\" + presetId)), 0, new AimpDlnaGroupingTreeDataProvider(mediaBrowser), &preset))) {
			preset->Release();
			result = S_OK;
		}
		break;
	}
	default:
		break;
	}

	Presets->Release();
	return result;
}

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
		*Value = AimpString(Plugin::Lang(id + L"\\Caption"), true);
	}

	return S_OK;
}

HRESULT WINAPI AimpDlnaDataStorage::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLDataProvider) {
		*ppvObject = dataProvider;
		dataProvider->AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}