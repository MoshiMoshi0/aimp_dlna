#pragma once

class AimpDlnaAlbumArtProvider : public IUnknownInterfaceImpl<IAIMPMLAlbumArtProvider2, IID_IAIMPMLAlbumArtProvider2> {
private:
	shared_ptr<PLT_SyncMediaBrowser> mediaBrowser;

public:
	AimpDlnaAlbumArtProvider(shared_ptr<PLT_SyncMediaBrowser> mediaBrowser) {
		this->mediaBrowser = mediaBrowser;
	}

	~AimpDlnaAlbumArtProvider() {
		mediaBrowser = nullptr;
	}

	HRESULT WINAPI Get(IAIMPObjectList* Fields, VARIANT* Values, IAIMPAlbumArtRequest* Request, IAIMPImageContainer** Image);
};