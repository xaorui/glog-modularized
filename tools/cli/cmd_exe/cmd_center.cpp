#include "cmd_center.h"
#include "cli_command.h"
#include "module.h"
#include "parameter_server.h"

#include <functional>

namespace tools {
namespace cli {

bool cmd_init(void)
{
    CmdCenter::load_config();
    cmd_setup("echo", echo_test, "echo testing");
    cmd_setup("module", CmdCenter::module, CmdCenter::module_help);
    cmd_setup("param", CmdCenter::param, CmdCenter::param_help);
    //cmd_setup("module", CmdCenter::module, "set module log switch");

    //cmd_setup("log_sw", CmdCenter::log_sw, "set module log switch");
    //cmd_setup("log_level", CmdCenter::log_level, "set module log level");
    //cmd_setup("log_config", CmdCenter::log_config, "get module log config");
    
    return true;
}

void CmdCenter::load_config()
{

}

bool CmdCenter::module(std::vector<std::string> cmd, std::ostringstream &out) {
    return base::Module::get_instance()->cmd(cmd, out);
}

bool CmdCenter::module_help(std::ostringstream &out) {
    return base::Module::get_instance()->cmd_help(out);
}

bool CmdCenter::param(std::vector<std::string> cmd, std::ostringstream &out) {
    return base::ParameterServer::instance().cmd(cmd, out);
}

bool CmdCenter::param_help(std::ostringstream &out) {
    return base::ParameterServer::instance().cmd_help(out);
}


}
}
