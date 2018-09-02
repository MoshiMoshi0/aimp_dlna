#pragma once

class AimpUtils {
private:
	static IAIMPCore *core;

public:
	static inline HRESULT CreateObject(REFIID IID, void **Obj){	return core->CreateObject(IID, Obj); }
	static inline void Unused(IUnknown* o){	o->AddRef(); o->Release(); }

	static inline void Initialize(IAIMPCore *Core) { core = Core; }
};

