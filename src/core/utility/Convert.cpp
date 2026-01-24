#include "Convert.h"
#include <memory>
#include <windows.h>

constexpr std::uint32_t CP_932 = 932;

std::optional<std::u8string> to_utf8(const std::string& str, const std::uint32_t code_page) {
    if(str.empty()) {
        return u8"";
    }

    const char* src = str.c_str();

    // UTF-16
    const int src_size = MultiByteToWideChar(
        code_page,
        MB_ERR_INVALID_CHARS,
        src,
        static_cast<int>(str.size()),
        NULL,
        0
    );
    if(src_size == 0) {
        return std::nullopt;
    }
    std::unique_ptr<wchar_t[]> utf_16_buffer = std::make_unique<wchar_t[]>(src_size);
    const int to_wide_result = MultiByteToWideChar(
        code_page,
        MB_ERR_INVALID_CHARS,
        src,
        static_cast<int>(str.size()),
        utf_16_buffer.get(),
        src_size
    );
    if(to_wide_result == 0) {
        return std::nullopt;
    }

    // UTF-16 -> UTF-8
    const int utf16_size = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        utf_16_buffer.get(),
        src_size,
        NULL,
        0,
        NULL,
        NULL
    );
    if(utf16_size == 0) {
        return std::nullopt;
    }
    std::unique_ptr<char[]> utf_8_buffer = std::make_unique<char[]>(static_cast<size_t>(utf16_size) + 1);
    const int to_multi_result = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        utf_16_buffer.get(),
        src_size,
        utf_8_buffer.get(),
        utf16_size,
        NULL,
        NULL
    );
    if(to_multi_result == 0) {
        return std::nullopt;
    }

    return std::u8string(
        reinterpret_cast<const char8_t*>(utf_8_buffer.get()),
        reinterpret_cast<const char8_t*>(utf_8_buffer.get() + utf16_size - 1)
    );
}

std::optional<std::u8string> sjis_to_utf8(const std::string& str) {
    return to_utf8(str, CP_932);
}