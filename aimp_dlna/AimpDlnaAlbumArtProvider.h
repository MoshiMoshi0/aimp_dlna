#pragma once

class AimpDlnaAlbumArtProvider : public IUnknownInterfaceImpl<IAIMPExtensionAlbumArtProvider, IID_IAIMPExtensionAlbumArtProvider> {
private:
	static AimpDlnaAlbumArtProvider* singleton;

	AimpDlnaAlbumArtProvider() {}

public:
	static AimpDlnaAlbumArtProvider* instance() {
		if (!singleton)
			singleton = new AimpDlnaAlbumArtProvider();

		return singleton;
	}

	~AimpDlnaAlbumArtProvider() {
		singleton = nullptr;
	}

	HRESULT WINAPI Get(IAIMPString *FileURI, IAIMPString *Artist, IAIMPString *Album, IAIMPPropertyList *Options, IAIMPImageContainer **Image);
	DWORD WINAPI GetCategory() { return AIMP_ALBUMART_PROVIDER_CATEGORY_INTERNET; }
};