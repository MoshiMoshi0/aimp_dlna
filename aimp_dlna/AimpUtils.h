#pragma once

#include "StringUtils.h"

class AimpUtils {
private:
	static IAIMPCore *core;
	static IAIMPServiceMUI* muiService;

public:
	static HRESULT Initialize(IAIMPCore *Core) {
		core = Core;

		if (FAILED(core->QueryInterface(IID_IAIMPServiceMUI, reinterpret_cast<void**>(&muiService)))) {
			return E_FAIL;
		}
		return S_OK;
	}

	static void Finalize() {
		if (muiService) {
			muiService->Release();
			muiService = nullptr;
		}

		core = nullptr;
	}

	static inline HRESULT CreateObject(REFIID IID, void **Obj){	return core->CreateObject(IID, Obj); }

	template<typename T>
	static T* CreateObject(REFIID IID) {
		T* obj = nullptr;
		if (SUCCEEDED(CreateObject(IID, reinterpret_cast<void**>(&obj))))
			return obj;
		return nullptr;
	}

	static vector<wstring> ToWideStringList(IAIMPObjectList* o) {
		vector<wstring> list;
		for (size_t i = 0; i < (size_t)o->GetCount(); i++) {
			IAIMPString* value = nullptr;
			if (SUCCEEDED(o->GetObject(i, IID_IAIMPString, reinterpret_cast<void**>(&value)))) {
				list.push_back(StringUtils::ToWideString(*value));
				value->Release();
			}
		}

		return list;
	}

	static wstring Lang(const wstring &Key, int Part = -1) {
		wstring ret;
		if (!muiService)
			return ret;

		IAIMPString *value = nullptr;
		if (Part > -1) {
			if (SUCCEEDED(muiService->GetValuePart(AimpString(Key), Part, &value))) {
				ret = value->GetData();
				value->Release();
			}
		} else {
			if (SUCCEEDED(muiService->GetValue(AimpString(Key), &value))) {
				ret = value->GetData();
				value->Release();
			}
		}
		return ret;
	}

	static inline void Unused(IUnknown* o){	o->AddRef(); o->Release(); }
};

