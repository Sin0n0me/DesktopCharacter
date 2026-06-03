#pragma once
#include "../math/WrappedMatrix.h"
#include <string>

class Script {
public:

    static void script_compile(const std::u8string& code);

    //static void script_execute(const char* str);

    static void run_demo(MMDMatrix& matrix);
};
