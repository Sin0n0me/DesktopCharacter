#include "Logger.h"
#include <chrono>
#include <iostream>

decltype(Logger::instance) Logger::instance = Logger{};

Logger::Logger(void) {
    this->is_running = true;
    this->file_stream.open("log.txt");
    this->worker = std::thread(&Logger::logging, this);
}

Logger::~Logger(void) noexcept {
    this->shutdown();
    this->file_stream.close();
}

void Logger::logging(void) {
    while(this->is_running) {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        this->condition.wait(
            lock,
            [this] {
                return !this->is_running || !this->queue.empty();
            }
        );

        while(!this->queue.empty()) {
            const LogEntry entry = std::move(this->queue.front());
            this->queue.pop();
            lock.unlock();

            this->write(entry);

            lock.lock();
        }
    }
}

void Logger::shutdown(void) {
    this->is_running = false;

    this->condition.notify_all();
    if(this->worker.joinable()) {
        this->worker.join();
    }
}

void Logger::enqueue(const LogLevel& level, const std::u8string& msg) {
    {
        std::lock_guard<std::mutex> lock(this->queue_mutex);
        this->queue.push(
            LogEntry{
                level,
                msg,
                std::chrono::system_clock::now()
            }
        );
    }

    this->condition.notify_one();
}

void Logger::write(const LogEntry& log_entry) {
    std::lock_guard<std::mutex> lock(this->output_mutex);

    const auto log = log_entry.format_log();

    if(this->file_stream.is_open()) {
        this->file_stream.write(
            reinterpret_cast<const char*>(log.data()),
            static_cast<std::streamsize>(log.size())
        );
        this->file_stream.flush();
    }

    std::cout.write(reinterpret_cast<const char*>(
        log.data()),
        static_cast<std::streamsize>(log.size())
    );
}

void Logger::debug(const std::u8string& msg) {
    Logger::instance.enqueue(
        LogLevel::Debug,
        msg
    );
}

void Logger::info(const std::u8string& msg) {
    Logger::instance.enqueue(
        LogLevel::Info,
        msg
    );
}

void Logger::warning(const std::u8string& msg) {
    Logger::instance.enqueue(
        LogLevel::Warning,
        msg
    );
}

void Logger::error(const std::u8string& msg) {
    Logger::instance.enqueue(
        LogLevel::Error,
        msg
    );
}

#ifdef _WIN32
#include <system_error>
std::u8string hresult_message(const HRESULT& hr) {
    const auto message = std::system_category().message(hr);
    return std::u8string(message.begin(), message.end());
}

#endif // _WIN32