#pragma once
#include <expected>
#include <string>
#include <vector>

namespace enishi ::foundation {
    template <typename E> class Error {
      private:
        std::vector<std::u8string> message;
        E error;

      public:
        explicit Error(const E error)
            : error(error)
            , message() {
        }
        explicit Error(const E error, const std::u8string& message)
            : error(error)
            , message() {
            this->add_message(message);
        }

        void add_message(const std::u8string& message) {
            this->message.push_back(message);
        }
    };
} // namespace enishi::foundation
