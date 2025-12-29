#pragma once

typedef HWND;
typedef UINT;

class IEngineStarter {
public:
	virtual ~IEngineStarter(void) = default;

	virtual bool init(const HWND& hwnd, const UINT& width, const UINT& height) = 0;

	virtual void run(void) = 0;

	virtual void uninit(void) = 0;
};
