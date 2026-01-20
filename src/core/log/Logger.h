#pragma once
#include "LogEntry.h"
#include "LogLevel.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <type_traits>

namespace detail {
    template <typename T>
    struct IsAllowedInteger : std::bool_constant<
        std::is_same_v<T, int8_t> ||
        std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t> ||
        std::is_same_v<T, uint8_t> ||
        std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, uint64_t>
    > {
    };

    template <typename T>
    constexpr bool IS_ALLORWD_INTERGER = IsAllowedInteger<std::remove_cv_t<T>>::value;

    template <typename T>
    constexpr bool IS_CHAR8_STRING = std::is_same_v<std::remove_cv_t<T>, const char8_t*> ||
        (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char8_t>);

    template <typename T>
    constexpr bool IS_U8STRING = std::is_same_v<std::remove_cv_t<T>, std::u8string>;
}

class Logger {
private:
    std::ofstream file_stream;
    std::queue<LogEntry> queue;
    std::mutex queue_mutex;
    std::mutex output_mutex;
    std::thread worker;
    std::condition_variable condition;
    std::atomic<bool> is_running;

    static Logger instance;

private:
    explicit Logger(void);
    ~Logger(void) noexcept;
    explicit Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logging(void);
    void shutdown(void);

    void enqueue(const LogLevel& level, const std::u8string& msg);
    void write(const LogEntry& log);

public:

    static void debug(const std::u8string& msg);
    static void info(const std::u8string& msg);
    static void warning(const std::u8string& msg);
    static void error(const std::u8string& msg);

    template <typename... Args>
    static std::u8string make_message(const Args&... args) {
        static_assert(
            (... &&
                (
                    detail::IS_ALLORWD_INTERGER<Args> ||
                    detail::IS_CHAR8_STRING<Args> ||
                    detail::IS_U8STRING<Args>)
                ),
            "Args must be char8_t string types, std::u8string or fixed-width integer types"
            );

        std::u8string result;

        auto append_one = [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            using Promoted = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>; // int8_t, uint8_t対策(必ず数値として扱う)

            if constexpr(detail::IS_U8STRING<T>) {
                result.append(value);
            } else if constexpr(detail::IS_CHAR8_STRING<T>) {
                result.append(value);
            } else if constexpr(detail::IS_ALLORWD_INTERGER<T>) {
                const std::string tmp = std::format("{}", static_cast<Promoted>(value));

                result.append(
                    reinterpret_cast<const char8_t*>(tmp.data()),
                    reinterpret_cast<const char8_t*>(tmp.data() + tmp.size())
                );
            }
            };

        (append_one(args), ...);

        return result;
    }
};
