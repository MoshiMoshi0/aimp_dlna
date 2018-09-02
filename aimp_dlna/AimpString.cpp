#include "stdafx.h"
#include "AIMPString.h"

IAIMPCore *AimpString::core = nullptr;

AimpString::AimpString() : string(nullptr) {
	if (!core)
		return;

	core->CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string));
}

AimpString::AimpString(const wstring &_string, bool addRef) : string(nullptr) {
	if (!core)
		return;

	if (SUCCEEDED(core->CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string)))) {
		string->SetData(const_cast<wchar_t *>(_string.data()), _string.size());

		if (addRef)
			string->AddRef();
	}
}

AimpString::AimpString(const wchar_t *_string, bool addRef) : string(nullptr) {
	if (!core)
		return;

	if (SUCCEEDED(core->CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string)))) {
		string->SetData(const_cast<wchar_t *>(_string), wcslen(_string));

		if (addRef)
			string->AddRef();
	}
}

AimpString::~AimpString() {
	if (string != nullptr)
		string->Release();
}