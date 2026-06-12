#pragma once
#include <optional>
#include <string>
#include <vector>

namespace enishi::foundation {
    namespace {
        template <typename T> class OptionBase : public std::optional<T> {
          public:
            using std::optional<T>::optional;

            [[nodiscard]] constexpr bool is_some(void) const {
                return this->has_value();
            }

            [[nodiscard]] constexpr bool is_none(void) const {
                return !this->has_value();
            }
        };
    } // namespace

    template <typename T> class Option : public OptionBase<T> {
      public:
        using OptionBase<T>::OptionBase;

        [[nodiscard]] T unwrap(void) const {
            return this->value();
        }
    };

    template <typename T> class Option<T&> : public OptionBase<std::reference_wrapper<T>> {
      public:
        using OptionBase<std::reference_wrapper<T>>::OptionBase;

        [[nodiscard]] T& unwrap(void) const {
            return this->value().get();
        }
    };
} // namespace enishi::foundation