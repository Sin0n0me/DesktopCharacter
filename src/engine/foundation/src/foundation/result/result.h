#pragma once
#include "../errors/errors.h"
#include <expected>
#include <string>
#include <vector>

namespace enishi::foundation {
    namespace {
        template <typename T, typename E> class ResultBase : public std::expected<T, E> {
          public:
            using std::expected<T, E>::expected;

            [[nodiscard]] constexpr bool is_ok(void) const {
                return this->has_value();
            }

            [[nodiscard]] constexpr bool is_err(void) const {
                return !this->has_value();
            }
        };
    } // namespace

    template <typename T, typename E> class Result : public ResultBase<T, E> {
      public:
        using ResultBase<T, E>::ResultBase;
    };

    template <typename E> class Result<void, E> : public ResultBase<void, E> {
      public:
        using ResultBase<void, E>::ResultBase;
    };

    template <typename T, typename E> class Result<T, Error<E>> : public ResultBase<T, Error<E>> {
      public:
        using ResultBase<T, Error<E>>::ResultBase;

        Result(const Error<E>& err)
            : ResultBase<T, Error<E>>(std::unexpected(err)) {
        }

        template <typename U>
        [[nodiscard]] constexpr Result<T, Error<U>> propagation(const U e) const {
            return std::unexpected(std::move(this->error().propagation(e)));
        }

        Result& add_message(const std::u8string& message) {
            if (this->is_ok()) {
                return *this;
            }
            this->error().add_message(message);

            return *this;
        }
    };

    template <typename E> class Result<void, Error<E>> : public ResultBase<void, Error<E>> {
      public:
        using ResultBase<void, Error<E>>::ResultBase;

        Result(const Error<E>& err)
            : ResultBase<void, Error<E>>(std::unexpected(err)) {
        }

        template <typename U>
        [[nodiscard]] constexpr Result<void, Error<U>> propagation(const U e) const {
            return std::unexpected(std::move(this->error().propagation(e)));
        }

        Result& add_message(const std::u8string& message) {
            if (this->is_ok()) {
                return *this;
            }
            this->error().add_message(message);

            return *this;
        }
    };

    template <typename T, typename E> using EngineResult = Result<T, Error<E>>;
    template <typename E> using VoidResult = EngineResult<void, E>;
} // namespace enishi::foundation
