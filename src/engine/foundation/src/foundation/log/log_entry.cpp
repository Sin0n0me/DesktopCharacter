#include "log_entry.h"
#include <format>
#include <ranges>
#include <string_view>

namespace enishi ::foundation {
    std::u8string add_indent(const std::u8string_view& input, std::u8string_view prefix) {
        auto lines = input | std::views::split('\n');
        std::u8string result;

        for (auto it = lines.begin(); it != lines.end(); ++it) {
            const std::u8string_view line{(*it).data(), (*it).size()};

            if (it != lines.begin()) {
                result += prefix;
            }
            result += line;
            result += '\n';
        }
        return result;
    }

    std::u8string LogEntry::format_log(void) const {
        // https://cpprefjp.github.io/reference/chrono/format.html
        const std::string time = std::format("[{:%F %T}]", this->time);
        // ASCII前提なので仮にASCIIを返さなかった場合はバグるので要修正
        const std::u8string time_stamp(time.begin(), time.end());
        const std::u8string log_level = u8"[" + level_to_string(this->level) + u8"]";
        const std::u8string indent(time_stamp.length() + log_level.length() + 2, ' ');

        return time_stamp + u8" " + log_level + u8" " + add_indent(this->message, indent);
    }
} // namespace enishi::foundation