#pragma once

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <string.h>

namespace tools {
namespace cli {
typedef bool (*Cmd_Exec)(std::vector <std::string>, std::ostringstream &out);
typedef bool (*Cmd_Help)(std::ostringstream &out);

bool echo_test(std::vector<std::string> cmd, std::ostringstream &out);

bool cmd_setup(std::string funcname, Cmd_Exec exec, std::string cmd_des = "");
bool cmd_setup(std::string funcname, Cmd_Exec exec, Cmd_Help help,
    std::string cmd_des = "");

bool cmd_exec(std::vector <std::string> cmd, std::ostringstream &out);

class CLIcommand {
public:
    static CLIcommand *get_instance() {
        static CLIcommand s_cli_command;
        return &s_cli_command;
    }

    bool add_cmd(std::string funcname, Cmd_Exec cmd_exec, Cmd_Help help, 
        std::string func_des = "");
    bool add_cmd(std::string funcname, Cmd_Exec cmd_exec, std::string func_des = "");

    bool exe_cmd(std::vector <std::string> args, std::ostringstream &out);

private:
    CLIcommand() {};
    std::map <std::string, Cmd_Exec> _cmd_map;
    std::map <std::string, Cmd_Help> _cmd_help_map;
    std::map <std::string, std::string> _cmd_des_map;
};
}
}
