#include "stdafx.h"
#include "AIMPString.h"

AimpString::AimpString(bool addRef) : string(nullptr) {
	string = AimpUtils::CreateObject<IAIMPString>(IID_IAIMPString);
	if (string != nullptr) {
		if(addRef)
			string->AddRef();
	}
}

AimpString::AimpString(const wstring &_string, bool addRef) : string(nullptr) {
	string = AimpUtils::CreateObject<IAIMPString>(IID_IAIMPString);
	if (string != nullptr) {
		string->SetData(const_cast<wchar_t *>(_string.data()), _string.size());

		if (addRef)
			string->AddRef();
	}
}

AimpString::AimpString(const wchar_t *_string, bool addRef) : string(nullptr) {
	string = AimpUtils::CreateObject<IAIMPString>(IID_IAIMPString);
	if (string != nullptr) {
		string->SetData(const_cast<wchar_t *>(_string), wcslen(_string));

		if (addRef)
			string->AddRef();
	}
}

AimpString::~AimpString() {
	if (string != nullptr)
		string->Release();
}