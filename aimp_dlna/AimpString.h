#pragma once

using namespace std;

class AimpString {
private:
	IAIMPString *string;

public:
	AimpString();
	AimpString(const wstring &string, bool addRef = false);
	AimpString(const wchar_t *string, bool addRef = false);
	
	~AimpString();

	operator IAIMPString*() { return string; }
	IAIMPString* operator->() { return string; }
};