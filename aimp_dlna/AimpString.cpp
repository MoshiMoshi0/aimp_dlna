#include "stdafx.h"
#include "AIMPString.h"

AimpString::AimpString() : string(nullptr) {
	AimpUtils::CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string));
}

AimpString::AimpString(const wstring &_string, bool addRef) : string(nullptr) {
	if (SUCCEEDED(AimpUtils::CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string)))) {
		string->SetData(const_cast<wchar_t *>(_string.data()), _string.size());

		if (addRef)
			string->AddRef();
	}
}

AimpString::AimpString(const wchar_t *_string, bool addRef) : string(nullptr) {
	if (SUCCEEDED(AimpUtils::CreateObject(IID_IAIMPString, reinterpret_cast<void **>(&string)))) {
		string->SetData(const_cast<wchar_t *>(_string), wcslen(_string));

		if (addRef)
			string->AddRef();
	}
}

AimpString::~AimpString() {
	if (string != nullptr)
		string->Release();
}