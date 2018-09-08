#pragma once

#include <codecvt>

class StringUtils {
private:
	static wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

public:
	static inline wstring ToWideString(const wchar_t* s) { return wstring(s); }
	static inline wstring ToWideString(const char* s) {	return converter.from_bytes(s);	}
	static inline wstring ToWideString(const string& s) { return converter.from_bytes(s); }
	static inline wstring ToWideString(const NPT_String& s) { return ToWideString(s.GetChars()); }
	static inline wstring ToWideString(IAIMPString& s) { return ToWideString(s.GetData()); }

	static inline string ToString(const wchar_t* s) { return converter.to_bytes(s); }
	static inline string ToString(const wstring& s) { return converter.to_bytes(s); }
	static inline string ToString(IAIMPString& s) { return ToString(s.GetData()); }

	static wchar_t* ToWideCharArray(const NPT_String& s) {
		auto length = s.GetLength();
		wchar_t* buffer = new wchar_t[length];
		ToWideString(s).copy(buffer, length);
		return buffer;
	}
};

