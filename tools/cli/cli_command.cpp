#include "cli_command.h"
namespace tools {
namespace cli {
bool echo_test(std::vector<std::string> cmd, std::ostringstream &out)
{
    for (auto & cm : cmd) {
        out << cm << "\n";
    }
    return true;
}

bool cmd_setup(std::string funcname, Cmd_Exec exec, std::string cmd_des)
{
    return CLIcommand::get_instance()->add_cmd(funcname, exec,cmd_des);
}
bool cmd_setup(std::string funcname, Cmd_Exec exec, Cmd_Help cmd_help, std::string cmd_des)
{
    return CLIcommand::get_instance()->add_cmd(funcname, exec, cmd_help, cmd_des);
}
bool cmd_exec(std::vector<std::string> cmd, std::ostringstream &out)
{
    return CLIcommand::get_instance()->exe_cmd(cmd, out);
}

bool CLIcommand::add_cmd(std::string funcname,  Cmd_Exec cmd_exec, Cmd_Help cmd_help,
    std::string func_des) {

    if (add_cmd(funcname, cmd_exec, func_des) == false) {
        return false;
    }
    _cmd_help_map[funcname] = cmd_help;
    return true;
}
bool CLIcommand::add_cmd(std::string funcname,  Cmd_Exec cmd_exec, std::string func_des) {
    auto cmd = _cmd_map.find(funcname);
    if (cmd != _cmd_map.end()) {
        return false;
    }
    _cmd_map[funcname] = cmd_exec;
    _cmd_des_map[funcname] = func_des;
    return true;
}

bool CLIcommand::exe_cmd(std::vector<std::string> args, std::ostringstream &out) {
    if (args.size() <= 0) {
        out << "empty args\n";
        return false;
    }
    if (args.size() == 1 && (strcmp(args[0].c_str(),"help") == 0
                            || strcmp(args[0].c_str(),"h") == 0
                            || strcmp(args[0].c_str(),"?") == 0)) {
        for (auto  &cmd : _cmd_map)
        {
            out << cmd.first << " : " <<_cmd_des_map[cmd.first] <<"\n";
            auto cmd_help = _cmd_help_map.find(cmd.first);
            if (cmd_help != _cmd_help_map.end()) {
                //_cmd_help_map[cmd.first](out);
                cmd_help->second(out);
            }
        }
        return true;
    }
    std::map<std::string, Cmd_Exec>::iterator itor;
    itor = _cmd_map.find(args[0]);
    if (itor == _cmd_map.end()) {
        out << "unknown cmd\n";
        return false;
    }
    //out << _cmd_des_map[itor->first] << "\n";
    return itor->second(args,out);
}

}
}

