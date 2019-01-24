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
	static inline string ToString(const NPT_String& s) { return string(s.GetChars()); }
	static inline string ToString(IAIMPString& s) { return ToString(s.GetData()); }

	static inline wchar_t* ToWideCharArray(const NPT_String& s) { return ToWideCharArray(ToWideString(s)); }
	static wchar_t* ToWideCharArray(const wstring& s) {
		wchar_t* buffer = new wchar_t[s.size() + 1];
		s.copy(buffer, s.size());
		buffer[s.size()] = '\0';
		return buffer;
	}


	static wstring Replace(const wstring& s, const wstring& find, const wstring& replace) {
		wstring result = s;
		size_t pos = 0;

		while (true) {
			pos = result.find(find, pos);
			if (pos == wstring::npos)
				break;

			result.erase(pos, find.length());
			result.insert(pos, replace);
		}

		return result;
	}

	static vector<wstring> Split(const wstring& s, const wstring& delimeter) {
		vector<wstring> result;
		size_t pos = 0;

		while (true) {
			auto fpos = s.find(delimeter, pos);
			if (fpos == wstring::npos) {
				result.push_back(s.substr(pos));
				break;
			}

			result.push_back(s.substr(pos, fpos - pos));
			pos = fpos + delimeter.size();
		}

		return result;
	}

	static bool EndsWith(const wstring& s, const wstring& with, const bool& ignoreCase = true) {
		if (with.size() > s.size())
			return false;

		if(ignoreCase)
			return equal(s.end() - with.size(), s.end(), with.begin(), with.end(),
				[](wchar_t a, wchar_t b) {
					return towlower(a) == towlower(b);
				});

		return equal(s.end() - with.size(), s.end(), with.begin(), with.end());
	}
};

