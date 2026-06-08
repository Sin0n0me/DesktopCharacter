#pragma once
#include "log_entry.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace enishi::foundation {
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
    };
} // namespace enishi::foundation