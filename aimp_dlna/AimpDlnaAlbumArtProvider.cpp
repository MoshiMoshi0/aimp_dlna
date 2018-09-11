#include "stdafx.h"
#include "AimpDlnaAlbumArtProvider.h"

AimpDlnaAlbumArtProvider* AimpDlnaAlbumArtProvider::singleton = nullptr;

HRESULT WINAPI AimpDlnaAlbumArtProvider::Get(IAIMPString *FileURI, IAIMPString *Artist, IAIMPString *Album, IAIMPPropertyList *Options, IAIMPImageContainer **Image) {
	int isAllowed;
	int maxSize;
	if (FAILED(Options->GetValueAsInt32(AIMP_SERVICE_ALBUMART_PROPID_FIND_IN_INTERNET, &isAllowed)) ||
		FAILED(Options->GetValueAsInt32(AIMP_SERVICE_ALBUMART_PROPID_FIND_IN_INTERNET_MAX_FILE_SIZE, &maxSize))) {
		return E_FAIL;
	}

	return E_FAIL;
}