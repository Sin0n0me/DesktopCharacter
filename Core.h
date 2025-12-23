#pragma once
#include <Windows.h>

#ifdef _DEBUG
constexpr bool IS_DEBUG_MODE = true;
#else
constexpr bool IS_DEBUG_MODE = false;
#endif // _DEBUG

int run_app(const HWND hwnd, const UINT width, const  UINT height);
