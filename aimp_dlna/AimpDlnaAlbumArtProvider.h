#pragma once

class AimpDlnaAlbumArtProvider : public IUnknownInterfaceImpl<IAIMPMLAlbumArtProvider, IID_IAIMPMLAlbumArtProvider> {
private:
	PLT_SyncMediaBrowser* mediaBrowser;

public:
	AimpDlnaAlbumArtProvider(PLT_SyncMediaBrowser* mediaBrowser) {
		this->mediaBrowser = mediaBrowser;
	}

	HRESULT WINAPI Get(IAIMPObjectList* Fields, VARIANT* Values, IAIMPPropertyList* Options, IAIMPImageContainer** Image);
};