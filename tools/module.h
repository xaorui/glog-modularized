#pragma once

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include "mlog/glog/logging.h"
namespace base {
#ifdef USING_ZLOG
    enum class LogLevel {
        UNKNOWN = 0,
        DEBUG = 0x01,
        INFO = 0x01 << 1,
        NOTICE = 0x01 << 2,
        WARNING = 0x01 << 3,
        ERROR = 0x01 << 4,
        FATAL = 0x01 << 5,
    };
#else
    enum class LogLevel {
        UNKNOWN = -1,
        DEBUG = google::DEBUG,
        INFO = google::INFO,
        NOTICE = google::WARNING,
        WARNING = google::WARNING,
        ERROR = google::ERROR,
        FATAL = google::FATAL,
    };
#endif
class ModuleConfig {
public:
    void sw(bool sw) { _sw = sw;}
    bool sw() {return _sw;}
    void log_level(LogLevel level) {_log_level = level;}
    LogLevel log_level() {return _log_level;}
private:
    bool _sw;
    LogLevel _log_level;
};

class Module {
public:
    static Module* get_instance();
    void init();
    bool valid_module(std::string name);
    bool sw(std::string name, bool sw);
    bool sw(std::string name, std::string sw); 
    bool sw(bool sw);
    bool get_sw(std::string name);
    bool set_sw(std::string name, std::string sw, std::ostringstream& out);
    //bool log_level(std::string name, LogLevel log_level);
    //bool log_level(LogLevel log_level);
    LogLevel get_log_level(std::string name);
    bool set_log_level(std::string name, LogLevel log_level, std::ostringstream& out);
    LogLevel get_level(std::string level);
    bool get_module();
    bool get_config();
    bool get_config(std::ostringstream& out);
    bool regist(std::string name);
    bool regist(std::string name, ModuleConfig config);
    bool cmd(std::vector<std::string> cmd, std::ostringstream& out);
    bool cmd_help(std::ostringstream& out);
private:
    Module() {}
    ~Module() {}
    std::recursive_mutex _mtx;
    std::unordered_map<std::string, ModuleConfig> _module_map;
    std::map<std::string, std::string> _cmd_des;
    std::map<LogLevel, std::string> _log_level_map;

};

}
