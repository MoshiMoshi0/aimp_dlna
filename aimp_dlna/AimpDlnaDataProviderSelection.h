#pragma once

using namespace std;

class AimpDlnaDataProviderSelection : public IUnknownInterfaceImpl<IAIMPMLDataProviderSelection> {
private:
	unsigned int index;
	PLT_MediaObjectListReference items;
	vector<wstring> fields;

	PLT_MediaObject* GetItem(unsigned int index);
	wstring GetField(unsigned int index);

public:
	AimpDlnaDataProviderSelection(PLT_MediaObjectListReference items, vector<wstring> fields) {
		this->items = items;
		this->fields = fields;
		this->index = 0;

		AddRef();
	}

	~AimpDlnaDataProviderSelection() {
		items.Detach();
	}

	DOUBLE WINAPI GetValueAsFloat(int FieldIndex);
	int WINAPI GetValueAsInt32(int FieldIndex);
	INT64 WINAPI GetValueAsInt64(int FieldIndex) { return INT64(); }
	WCHAR* WINAPI GetValueAsString(int FieldIndex, int* Length);

	BOOL WINAPI HasNextPage() { return false; }
	BOOL WINAPI NextRow();

	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject);
};

