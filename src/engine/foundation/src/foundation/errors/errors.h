#pragma once
#include "../str/str.h"
#include <expected>
#include <vector>

namespace enishi ::foundation {
    template <typename E> class Error {
      private:
        std::vector<UTF8> message;
        E error;

      private:
        template <typename U> friend class Error;

      public:
        explicit Error(const E error)
            : error(error)
            , message() {
        }
        explicit Error(const E error, const UTF8& message)
            : error(error)
            , message() {
            this->add_message(message);
        }

        void add_message(const UTF8& message) {
            this->message.push_back(message);
        }

        template <typename T> Error<T> propagation(const T new_error) const {
            Error<T> next_err = Error<T>(new_error);
            next_err.message = this->message;
            return next_err;
        }
    };
} // namespace enishi::foundation
