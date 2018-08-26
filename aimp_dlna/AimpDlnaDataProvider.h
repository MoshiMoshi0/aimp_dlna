#pragma once

class AimpDlnaDataProvider : public IUnknownInterfaceImpl<IAIMPMLDataProvider> {
public:
	HRESULT WINAPI GetData(IAIMPObjectList* Fields, IAIMPMLDataFilter* Filter, IUnknown** Data);
};