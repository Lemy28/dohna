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

    static Logger& getInstance( LogLevel logLevel = LogLevel::Info,const std::string& logFile="./dohna.log");
    void Log(LogLevel logLevel, const char* format, ...);
    void Log(const char* format,LogLevel logLevel = LogLevel::Info); 



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
