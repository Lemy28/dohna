

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <unordered_map>
#include <string>


namespace dohna{

using Configs = std::unordered_map<std::string,std::string> ;

void parse_config_file(const std::string& filename,Configs& configs);


}




#endif