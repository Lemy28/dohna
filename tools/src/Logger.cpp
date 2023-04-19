#include <tools/Logger.h>
#include <stdarg.h>
#include <tools/ConfigManager.h>


Logger& Logger::getInstance(const std::string& logfile) {
    ConfigManager& cm = ConfigManager::getInstance();
    cm.loadConfig("/etc/dohna.conf");
    //根据配置文件中的loglevel来初始化logger
    static Logger logger(logfile, static_cast<LogLevel>(cm.getInt("logLevel")));
    return logger;
}


void Logger::LogError(const char* format){
    Log(LogLevel::Error,format);
}

void Logger::LogInfo(const char* format){
    Log(LogLevel::Info,format);
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
            exit(EXIT_FAILURE);
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
