#include "Utility.h"
#include <windows.h>

std::wstring chat_to_wstring(const char* text) {
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);

	if(requiredSize == 0) {
		return L"";
	}

	std::wstring wstr(requiredSize, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, text, -1, &wstr[0], requiredSize);

	wstr.resize(static_cast<size_t>(requiredSize) - 1);

	return wstr;
}