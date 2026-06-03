#include "Script.h"

// Rust側関数の宣言
extern "C" __declspec(dllimport) bool compile(const char* code_ptr, size_t len);
extern "C" __declspec(dllimport) bool run_script(float* matrix_ptr);

#pragma comment(lib, "../Musubu/target/debug/musubu.dll.lib")

void Script::script_compile(const std::u8string& code) {
    if(!compile(reinterpret_cast<const char*>(code.c_str()), code.size())) {
        const auto err = 0;
    }
}

void Script::run_demo(MMDMatrix& matrix) {
    auto mat = matrix.to_column_major_array();
    if(run_script(mat.data())) {
        matrix = MMDMatrix(mat);
    }
}