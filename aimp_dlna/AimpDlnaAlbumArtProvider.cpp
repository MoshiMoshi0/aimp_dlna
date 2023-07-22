#include "stdafx.h"
#include "AimpDlnaAlbumArtProvider.h"
#include "AimpDlnaMusicLibrary.h"

HRESULT WINAPI AimpDlnaAlbumArtProvider::Get(IAIMPObjectList* Fields, VARIANT* Values, IAIMPAlbumArtRequest* Request, IAIMPImageContainer** Image) {
	string deviceUuid, objectId;
	auto fields = AimpUtils::ToWideStringList(Fields);
	for (auto field : fields) {
		if (field.compare(EVDS_DeviceUuid) == 0)
			deviceUuid = StringUtils::ToString(Values->bstrVal);
		else if (field.compare(EVDS_Id) == 0)
			objectId = StringUtils::ToString(Values->bstrVal);

		if (++Values == nullptr)
			break;
	}

	if (deviceUuid.size() == 0 || objectId.size() == 0)
		return E_FAIL;

	PLT_DeviceDataReference device;
	if (NPT_FAILED(mediaBrowser->FindServer(deviceUuid.c_str(), device)))
		return E_FAIL;

	PLT_MediaObjectListReference objects;
	if (NPT_FAILED(mediaBrowser->BrowseSync(device, objectId.c_str(), objects)))
		return E_FAIL;
	if (objects.IsNull() || (*objects).GetItemCount() == 0)
		return E_FAIL;

	auto item = *objects->GetFirstItem();
	if (item->m_ExtraInfo.album_arts.GetItemCount() == 0) {
		objects->Clear();

		if (NPT_FAILED(mediaBrowser->BrowseSync(device, item->m_ParentID, objects)))
			return E_FAIL;
		if (objects.IsNull() || (*objects).GetItemCount() == 0)
			return E_FAIL;

		item = *objects->GetFirstItem();
		if (item->m_ExtraInfo.album_arts.GetItemCount() == 0)
			return E_FAIL;
	}

	auto url = StringUtils::ToWideString((*item->m_ExtraInfo.album_arts.GetFirstItem()).uri);
	auto aimpUrl = AimpString(url, true);
	if (SUCCEEDED(Request->CacheGet(aimpUrl, Image)))
		return S_OK;

	IAIMPString* extensionsString;
	if (FAILED(Request->GetValueAsObject(AIMP_ALBUMART_REQUEST_PROPID_FIND_IN_FILES_EXTS, IID_IAIMPString, reinterpret_cast<void**>(&extensionsString))))
		return E_FAIL;

	auto extensionList = StringUtils::Split(extensionsString->GetData(), L";");
	extensionsString->Release();

	for (auto ext : extensionList) {
		if (ext.size() == 0)
			continue;

		ext.erase(0, 1);
		if (!StringUtils::EndsWith(url, ext, true))
			continue;

		if (!SUCCEEDED(Request->Download(aimpUrl, Image)))
			return E_FAIL;

		Request->CachePut(aimpUrl, Image);
		return S_OK;
	}

	return E_FAIL;
}