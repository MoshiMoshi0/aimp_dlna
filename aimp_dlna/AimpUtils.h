#pragma once

#include "StringUtils.h"

class AimpUtils {
private:
	static IAIMPCore *core;

public:
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

	static inline void Unused(IUnknown* o){	o->AddRef(); o->Release(); }

	static inline void Initialize(IAIMPCore *Core) { core = Core; }
};

