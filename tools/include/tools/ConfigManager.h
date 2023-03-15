#pragma once

#include <map>
#include <string>

class ConfigManager {
public:
    static ConfigManager& getInstance();
    bool loadConfig(const std::string& filename);
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
private:
    ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    std::map<std::string, std::string> configMap_;
};