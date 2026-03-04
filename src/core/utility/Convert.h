#pragma once

#include <optional>
#include <string>

std::optional<std::u8string> to_utf8(const std::string& str, const std::uint32_t code_page);

std::optional<std::u8string> sjis_to_utf8(const std::string& str);
