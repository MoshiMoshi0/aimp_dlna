#pragma once

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

	static inline void Unused(IUnknown* o){	o->AddRef(); o->Release(); }

	static inline void Initialize(IAIMPCore *Core) { core = Core; }
};

