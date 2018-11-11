#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace tools {
namespace cli {

bool cmd_init(void);

class CmdCenter {
public:
    static void load_config();
    static bool module(std::vector<std::string> cmd, std::ostringstream &out);
    static bool module_help(std::ostringstream &out);
    static bool param(std::vector<std::string> cmd, std::ostringstream &out);
    static bool param_help(std::ostringstream &out);
    
private:
};

}
}
