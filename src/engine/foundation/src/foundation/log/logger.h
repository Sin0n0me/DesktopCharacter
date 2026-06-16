#pragma once
#include "log_entry.h"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <queue>
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

        void enqueue(const LogLevel& level, const UTF8& msg);
        void write(const LogEntry& log);

      public:
        static void debug(const UTF8& msg);
        static void info(const UTF8& msg);
        static void warning(const UTF8& msg);
        static void error(const UTF8& msg);
    };
} // namespace enishi::foundation