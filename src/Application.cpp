#include "Application.h"
#include "core/Core.h"
#include "core/log/Logger.h"

int WINAPI wWinMain(
    const HINSTANCE hinstance,
    const HINSTANCE,
    const LPWSTR cmd_line,
    const int
) {
    Logger::info(u8"起動");
    Engine engine = Engine(hinstance, cmd_line);

    if(!engine.init(WIDTH, HEIGHT)) {
        Logger::error(u8"初期化に失敗しました");
        return -1;
    }

    engine.run();

    engine.uninit();

    return 0;
}