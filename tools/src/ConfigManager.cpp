// ConfigManager.cpp

#include <tools/ConfigManager.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <tools/Logger.h>
#include <algorithm>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr<<"Error:failed to load config file"<<std::endl;
        Logger::getInstance().Log(LogLevel::Error,"Erorr:failed to load config:dohna.conf");
        return false;
    }
    std::string line;
    while (std::getline(infile, line)) {//��infile�ж�ȡһ�е�line��

    //��ÿһ��ת��Ϊiss,�������getline����
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) { // '='��Ϊ�ָ��� ��iss������=ǰ��Ĳ��ֶ���key��

            char spacechar = ' ';
            auto newEnd = std::remove(key.begin(),key.end(),spacechar);
            key.erase(newEnd,key.end());

            std::string value;
            if (std::getline(iss, value)) {
                auto newEnd = std::remove(value.begin(),value.end(),spacechar);
                value.erase(newEnd,value.end());
                configMap_[key] = value;
            }
        }
    }

    // for(auto& i : configMap_){
    //     std::cout<<"first:"<<i.first<<";"<<std::endl;
    //     std::cout<<"second:"<<i.second<<";"<<std::endl;
    // }

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
