#pragma once

using namespace std;

template <typename T>
struct AimpGenericDataProviderSelectionSelectors {
	function<DOUBLE(T*, wstring, int)> FloatSelector;
	function<int(T*, wstring, int)> Int32Selector;
	function<INT64(T*, wstring, int)> Int64Selector;
	function<WCHAR*(T*, wstring, int, int*)> StringSelector;
};

template <typename T>
class AimpGenericDataProviderSelection : public IUnknownInterfaceImpl<IAIMPMLDataProviderSelection> {
private:
	int index;
	vector<T> items;
	vector<wstring> fields;
	AimpGenericDataProviderSelectionSelectors<T> selectors;

	T* GetCurrentItem() { return &items[index]; }
public:
	AimpGenericDataProviderSelection(vector<T> items, vector<wstring> fields, AimpGenericDataProviderSelectionSelectors<T> selectors) {
		this->items = items;
		this->fields = fields;
		this->selectors = selectors;
		this->index = 0;

		AddRef();
	}

	~AimpGenericDataProviderSelection() {}

	DOUBLE WINAPI GetValueAsFloat(int FieldIndex) { return selectors.FloatSelector(GetCurrentItem(), fields[FieldIndex], FieldIndex); }
	int WINAPI GetValueAsInt32(int FieldIndex) { return selectors.Int32Selector(GetCurrentItem(), fields[FieldIndex], FieldIndex); }
	INT64 WINAPI GetValueAsInt64(int FieldIndex) { return selectors.Int64Selector(GetCurrentItem(), fields[FieldIndex], FieldIndex); }
	WCHAR* WINAPI GetValueAsString(int FieldIndex, int* Length) { return selectors.StringSelector(GetCurrentItem(), fields[FieldIndex], FieldIndex, Length); }

	BOOL WINAPI HasNextPage() { return false; }

	BOOL WINAPI NextRow() {
		if (items.size() == 0)
			return false;

		auto result = ++index < items.size();
		if (!result)
			index = 0;

		return result;
	}

	HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppvObject) {
		if (riid == IID_IAIMPMLDataProviderSelection) {
			*ppvObject = this;
			AddRef();
			return S_OK;
		}

		return Base::QueryInterface(riid, ppvObject);
	}
};

