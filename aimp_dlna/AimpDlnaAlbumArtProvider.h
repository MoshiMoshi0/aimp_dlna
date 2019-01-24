#pragma once

class AimpDlnaAlbumArtProvider : public IUnknownInterfaceImpl<IAIMPMLAlbumArtProvider, IID_IAIMPMLAlbumArtProvider> {
private:
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

public:
	AimpDlnaAlbumArtProvider(shared_ptr<PLT_SyncMediaBrowser> mediaBrowser) {
		this->mediaBrowser = mediaBrowser;
	}

	~AimpDlnaAlbumArtProvider() {
		mediaBrowser = nullptr;
	}

	HRESULT WINAPI Get(IAIMPObjectList* Fields, VARIANT* Values, IAIMPPropertyList* Options, IAIMPImageContainer** Image);
};