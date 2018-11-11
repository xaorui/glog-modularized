#include <iomanip>
#include <cctype>
#include <algorithm>
#include "module.h"

namespace base {
Module* Module::get_instance() {
    static Module s_module;
    return &s_module;
}
void Module::init() {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    ModuleConfig config;
    config.sw(true);
    config.log_level(LogLevel::INFO);
    _module_map["default"] = config;

    _cmd_des["sw"] = "[module] on|off";
    _cmd_des["config"] = "get module config";
    _cmd_des["add"] = "add module";
    _cmd_des["log"] = "[module] (debug | info | notice | warning | error | fatal)";

    _log_level_map[LogLevel::DEBUG] = "debug";
    _log_level_map[LogLevel::INFO] = "info";
    _log_level_map[LogLevel::NOTICE] = "notice";
    _log_level_map[LogLevel::WARNING] = "warning";
    _log_level_map[LogLevel::ERROR] = "error";
    _log_level_map[LogLevel::FATAL] = "fatal";
}
bool Module::regist(std::string name) {
    ModuleConfig config;
    config.sw(true);
    config.log_level(LogLevel::INFO);
    return regist(name, config);
}
bool Module::regist(std::string name, ModuleConfig config) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    auto module = _module_map.find(name);
    if (module != _module_map.end()) {
        std::cout << "The same moduel has been registed!" << std::endl;
        return false;
    }
    _module_map[name] = config;

    google::module_reg(name);
    google::module_sw(name, config.sw());
    google::module_level(name, static_cast<int>(config.log_level()));
    return true;
}
bool Module::cmd(std::vector<std::string> cmd, std::ostringstream& out) {
    if (cmd[1] == "sw") {
        out << " ";
        if (cmd.size() == 3) {
            return set_sw(cmd[2], "", out);
        } else if (cmd.size() == 4) {
            return set_sw(cmd[2], cmd[3], out);
        } else {
            return get_config(out);
        }
    } else if (cmd[1] == "config") {
        return get_config(out);
    } else if (cmd[1] == "log") {
        if (cmd.size() == 3) {
            LogLevel level = get_level(cmd[2]);
            return set_log_level("all", level, out);
        }  else if (cmd.size() == 4) {
            LogLevel level = get_level(cmd[3]);
            return set_log_level(cmd[2], level, out);
        } else {
            return false;
        }
    } else if (cmd[1] == "add") {
        if (cmd.size() == 3) {
            return regist(cmd[2]);
        } else {
            return false;
        }
    }
    else {
        return false;
    }
}
bool Module::cmd_help(std::ostringstream& out) {
    for (auto cmd : _cmd_des) {
        out << "\t" << cmd.first << "\t" << cmd.second << "\n";
    }
    return true;
}
bool Module::valid_module(std::string name) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    auto module = _module_map.find(name);
    if (module == _module_map.end()) {
        return false;
    }
    return true;
}
bool Module::set_log_level(std::string name, LogLevel log_level, std::ostringstream& out) {
    bool res = false;
    if (log_level == LogLevel::UNKNOWN) {
        out << " log " <<  name << " " << "unknown log_level\n";
        return false;
    }
    if (name == "all") {
        for (auto & module : _module_map) {
            //_module_map[module.first].sw(sw);
            module.second.log_level(log_level);
            google::module_level(module.first, static_cast<int>(log_level));
        }
        res = true;
    } else {
        if (valid_module(name) == false) {
            out << " no such module" << "\n";
            return false;
        }
        _module_map[name].log_level(log_level);
        google::module_level(name, static_cast<int>(log_level));
        res = true;
    }
    out << " log " <<  name << " " <<  _log_level_map[log_level] << " " << (res == true ? "succeed" : "failed") << "\n";
    return res;
}

LogLevel Module::get_log_level(std::string name) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if (valid_module(name) == false) {
        return LogLevel::UNKNOWN;
    }
    return _module_map[name].log_level();
}
LogLevel Module::get_level(std::string level) {
    for (auto lev : _log_level_map) {
        std::transform(level.begin(), level.end(), level.begin(), ::tolower);
        if (lev.second == level) {
            return lev.first;
        }
    }
    return LogLevel::UNKNOWN;
}
bool Module::sw(std::string name, bool swi) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if (valid_module(name) == false) {
        return false;
    }
    _module_map[name].sw(swi);
    google::module_sw(name, swi);
    return true;
}
bool Module::get_sw(std::string name) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if (name == "on" || name == "off") {
        return false;
    }
    auto module = _module_map.find(name);
    if (module == _module_map.end()) {
        return false;
    }
    return _module_map[name].sw();
}
bool Module::sw(std::string name, std::string swi) {
    bool res = false;
    if (swi == "on") {
        res = sw(name, true);
    } else if (swi == "off") {
        res = sw(name, false);
    } else {
        return false;
    }
    return res;
}
bool Module::sw(bool sw) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    for (auto & module : _module_map) {
        //_module_map[module.first].sw(sw);
        module.second.sw(sw);
        google::module_sw(module.first, sw);
    }
    return true;
}
bool Module::set_sw(std::string name, std::string swi, std::ostringstream& out) {
    bool res = false;
    if (swi == "on" || swi == "off") {
        if (valid_module(name) == true) {
            res = sw(name, swi);
            out << "sw " << name << " " << swi << " " << (res == true ? "succeed" : "failed") << "\n";
        } else {
            out << "no such module" << "\n";
        }
    }  else if (swi == "") {
        /* sw all*/
        if (name == "on" || name == "off") {
            if (name == "on") {
                res = sw(true);
            } else {
                res = sw(false);
            }
            out << "sw " << "all " << name << " " << (res == true ? "succeed" : "failed") << "\n";
        } else {
            if (valid_module(name) == false) {
                out << "no such module" << "\n";
            } else {
                out << name << " sw " << (get_sw(name) == true ? "on" : "off") << "\n";
                return true;
            }
        }
    } else {
        out << "no support sw options" << "\n";
    }
    return res;
}

bool Module::get_module() {
    return get_config();
}
bool Module::get_config() {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    std::cout << std::setfill('_') << std::setw(64)  << "_" << std::endl;
    std::cout << std::setfill(' ') << std::setw(12)  << "name" <<
              std::setw(6) << "sw" << std::endl;
    for (auto module : _module_map) {
        std::cout << std::setw(12)  << module.first <<
                  std::setw(6) <<  ((module.second.sw() == true) ? "on" : "off") << std::endl;
    }
    return true;
}
bool Module::get_config(std::ostringstream& out) {
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    out << std::setfill('_') << std::setw(64)  << "_" << std::endl;
    out << std::setfill(' ') << std::setw(12)  << "name" <<
        std::setw(6) << "sw" <<
        std::setw(10) << "level" <<
        std::endl;
    for (auto module : _module_map) {
        out << std::setw(12)  << module.first <<
            std::setw(6) <<  ((module.second.sw() == true) ? "on" : "off") <<
            std::setw(10) << _log_level_map[module.second.log_level()] <<
            std::endl;
    }
    return true;
}

}