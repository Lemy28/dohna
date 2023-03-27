#include <tools/Logger.h>
#include <stdarg.h>


Logger& Logger::getInstance(const std::string& logfile, LogLevel loglevel) {
    static Logger logger(logfile,loglevel);
    return logger;
}

void Logger::Log(LogLevel logLevel, const char* format, ...) {
    if (logLevel >= m_logLevel) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::ofstream ofs(m_logFile, std::ios::app);
        if (ofs.is_open()) {
            char buffer[1024];
            va_list args;
            va_start(args, format);
            std::vsprintf(buffer, format, args);
            va_end(args);
            ofs << GetFormattedTime() << " [" << LogLevelString(logLevel) << "] " << buffer << std::endl;
        }
        else{
            std::cerr<<"Error:failed to open log file"<<std::endl;
        }
        ofs.close();
    }
}

Logger::Logger(const std::string& logFile, LogLevel logLevel) : m_logFile(logFile), m_logLevel(logLevel) {}

Logger::~Logger() {}

std::string Logger::GetFormattedTime() {
    std::time_t now = std::time(nullptr);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buffer;
}

std::string Logger::LogLevelString(LogLevel logLevel) {
    switch (logLevel) {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    default:
        return "";
    }
}
