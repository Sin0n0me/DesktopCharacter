#pragma once
#include <cstdint>
#include <format>
#include <string>
#include <type_traits>

namespace enishi ::foundation {
    namespace {
        template <typename T>
        struct IsAllowedInteger
            : std::bool_constant<
                  std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t> ||
                  std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::int64_t> ||
                  std::is_same_v<T, std::uint8_t> || std::is_same_v<T, std::uint16_t> ||
                  std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>> {};

        template <typename T>
        constexpr bool IS_ALLORWD_INTERGER = IsAllowedInteger<std::remove_cv_t<T>>::value;

        template <typename T>
        struct IsAllowedFloat
            : std::bool_constant<std::is_same_v<T, float> || std::is_same_v<T, double> ||
                                 std::is_same_v<T, long double> || std::is_same_v<T, float_t>> {};

        template <typename T>
        constexpr bool IS_ALLORWD_FLOAT = IsAllowedFloat<std::remove_cv_t<T>>::value;

        template <typename T>
        constexpr bool IS_CHAR8_STRING =
            std::is_same_v<std::remove_cv_t<T>, const char8_t*> ||
            (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char8_t>);

        template <typename T>
        constexpr bool IS_U8STRING = std::is_same_v<std::remove_cv_t<T>, std::u8string>;
    } // namespace

    template <typename... Args> [[nodiscard]] static std::u8string concat(const Args&... args) {
        static_assert((... && (IS_ALLORWD_INTERGER<Args> || IS_ALLORWD_FLOAT<Args> ||
                                  IS_CHAR8_STRING<Args> || IS_U8STRING<Args>)),
            "Args must be char8_t string types, std::u8string or fixed-width integer types");

        std::u8string result;

        auto append_one = [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            using Promoted = std::conditional_t<std::is_signed_v<T>,
                std::int64_t,
                std::uint64_t>; // int8_t, uint8_t対策(必ず数値として扱う)

            if constexpr (IS_U8STRING<T>) {
                result.append(value);
            } else if constexpr (IS_CHAR8_STRING<T>) {
                result.append(value);
            } else if constexpr (IS_ALLORWD_INTERGER<T>) {
                const std::string tmp = std::format("{}", static_cast<Promoted>(value));

                result.append(reinterpret_cast<const char8_t*>(tmp.data()),
                    reinterpret_cast<const char8_t*>(tmp.data() + tmp.size()));
            } else if constexpr (IS_ALLORWD_FLOAT<T>) {
                const std::string tmp = std::format("{:.8f}", value);

                result.append(reinterpret_cast<const char8_t*>(tmp.data()),
                    reinterpret_cast<const char8_t*>(tmp.data() + tmp.size()));
            }
        };

        (append_one(args), ...);

        return result;
    }
} // namespace enishi::foundation