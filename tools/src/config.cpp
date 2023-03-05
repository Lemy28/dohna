#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <tools/config.h>


namespace dohna{


void parse_config_file(const std::string& filename, Configs& configs)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            // Ignore empty lines and comments  ;在Windows的init配置文件中是注释
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Invalid line, skip it
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Trim leading and trailing whitespace from key and value
        size_t start = key.find_first_not_of(" \t");
        size_t end = key.find_last_not_of(" \t");
        key = key.substr(start, end - start + 1);

        start = value.find_first_not_of(" \t");
        end = value.find_last_not_of(" \t");
        value = value.substr(start, end - start + 1);

        configs[key] = value;
    }

    ifs.close();
}


}//namespace end

// int main()
// {
//     std::unordered_map<std::string, std::string> configs;
//     parse_config_file("example.conf", configs);

//     // Print the parsed configurations
//     for (const auto& pair : configs) {
//         std::cout << pair.first << " = " << pair.second << std::endl;
//     }

//     return 0;
// }
