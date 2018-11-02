#pragma once

class AimpDlnaAlbumArtProvider : public IUnknownInterfaceImpl<IAIMPExtensionAlbumArtProvider, IID_IAIMPExtensionAlbumArtProvider> {
public:
	HRESULT WINAPI Get(IAIMPString *FileURI, IAIMPString *Artist, IAIMPString *Album, IAIMPPropertyList *Options, IAIMPImageContainer **Image);
	DWORD WINAPI GetCategory() { return AIMP_ALBUMART_PROVIDER_CATEGORY_INTERNET; }
};