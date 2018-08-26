#pragma once

#include "stdafx.h"

using namespace std;

class AimpString {
private:
	static IAIMPCore *core;
	IAIMPString *string;

public:
	AimpString();
	AimpString(const wstring &string, bool addRef = false);
	AimpString(const wchar_t *string, bool addRef = false);
	
	~AimpString();

	operator IAIMPString*() { return string; }
	IAIMPString* operator->() { return string; }

	static inline void Initialize(IAIMPCore *Core) { core = Core; }
};
