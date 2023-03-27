#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <mutex>


enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};


class Logger {
public:

    static Logger& getInstance(const std::string& logFile="./dohna.log", LogLevel logLevel = LogLevel::Info);
    void Log(LogLevel logLevel, const char* format, ...);

    Logger& operator<<(const std::string msg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Log(m_logLevel, "%s",msg.c_str());
        return *this;
    }


    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger(const std::string& logFile, LogLevel logLevel);
    ~Logger();

    std::string GetFormattedTime(); 

    std::string LogLevelString(LogLevel logLevel);


    static Logger* s_instance;
    std::mutex m_mutex;
    std::string m_logFile;
    LogLevel m_logLevel;
};
