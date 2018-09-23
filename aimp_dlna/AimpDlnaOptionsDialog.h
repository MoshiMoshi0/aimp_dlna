#pragma once

class AimpDlnaOptionsDialog : public IUnknownInterfaceImpl<IAIMPOptionsDialogFrame, IID_IAIMPOptionsDialogFrame> {
private:
	IAIMPServiceOptionsDialog* dialogService;
	HWND handle;

	static BOOL CALLBACK DlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam); 
	static LRESULT CALLBACK FrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK GroupBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	inline void Dirty() { 
		if (dialogService)
			dialogService->FrameModified(this);
	}

public:
	AimpDlnaOptionsDialog(IAIMPCore* core) : dialogService(nullptr), handle(NULL) {
		core->QueryInterface(IID_IAIMPServiceOptionsDialog, reinterpret_cast<void **>(&dialogService));
	}

	~AimpDlnaOptionsDialog() {
		if (dialogService) {
			dialogService->Release();
			dialogService = nullptr;
		}
	}

	HRESULT WINAPI GetName(IAIMPString **S);
	HWND WINAPI CreateFrame(HWND ParentWnd);
	void WINAPI DestroyFrame();
	void WINAPI Notification(int ID);
};