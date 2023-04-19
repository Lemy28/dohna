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

    static Logger& getInstance(const std::string& logFile="./dohna.log");
    void Log(LogLevel logLevel, const char* format, ...);
    void LogError(const char* format);
    void LogInfo(const char* format);
    static std::string GetFormattedTime(); 


    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger(const std::string& logFile, LogLevel logLevel);
    ~Logger();


    std::string LogLevelString(LogLevel logLevel);


    static Logger* s_instance;
    std::mutex m_mutex;
    std::string m_logFile;
    LogLevel m_logLevel;
};
