// ConfigManager.cpp

#include <tools/ConfigManager.h>
#include <fstream>
#include <sstream>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(infile, line)) {//��infile�ж�ȡһ�е�line��

    //��ÿһ��ת��Ϊiss,�������getline����
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) { // '='��Ϊ�ָ��� ��iss���ݶ���key��
            std::string value;
            if (std::getline(iss, value)) {
                configMap_[key] = value;
            }
        }
    }
    infile.close();
    return true;
}

std::string ConfigManager::getString(const std::string& key) const {
    auto iter = configMap_.find(key);
    if (iter != configMap_.end()) {
        return iter->second;
    }
    return "";
}

int ConfigManager::getInt(const std::string& key) const {
    auto iter = configMap_.find(key);
    if (iter != configMap_.end()) {
        return std::stoi(iter->second);
    }
    return 0;
}
