#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include "mlog/glog/logging.h"

#include "cli/cli_server.h"
#include "cli/cli_command.h"
#include "cli/cmd_exe/cmd_center.h"

#include <sstream>

class LogHelper {
public:
    LogHelper(char*program) {
        google::InitGoogleLogging(program);
        FLAGS_stderrthreshold = google::INFO;
        FLAGS_colorlogtostderr = true;
        FLAGS_v = 3;
    }
    void module_reg(std::string mod) {
        google::module_reg(mod);
    }
    void module_sw(std::string mod, bool sw) {
        google::module_sw(mod, sw);
    }
    void module_level(std::string mod, int level) {
        google::module_level(mod, level);
    }
    ~LogHelper()
    {
        google::ShutdownGoogleLogging();
    }
};


namespace log {
enum Color {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
};

static Color level_to_color(int level) {
    //assert(level >= 0 && severity < 4);
    Color color = COLOR_DEFAULT;
    switch (level) {
    //case GLOG_DEBUG:
        //color = COLOR_GREEN;
        //break;
    case google::INFO:
        color = COLOR_DEFAULT;
        break;
    case google::WARNING:
        color = COLOR_YELLOW;
        break;
    case google::ERROR:
    case google::FATAL:
        color = COLOR_RED;
        break;
    default:
        // should never get here.
        //assert(false);
        color = COLOR_DEFAULT;
    }
    return color;
}

// Returns the ANSI color code for the given color.
const char* get_ansi_colorcode(Color color) {
    switch (color) {
        case COLOR_RED:     return "1";
        case COLOR_GREEN:   return "2";
        case COLOR_YELLOW:  return "3";
        case COLOR_DEFAULT:  return "";
    };
    return nullptr; // stop warning about return type.
}

std::string get_time()
{
    char temp[64] = {0};
#if 1
    char tmp[64] = {0};
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    time_t timep = std::chrono::system_clock::to_time_t(tp);
    std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::from_time_t(timep);

    int usecs = std::chrono::time_point_cast<std::chrono::microseconds>(tp).time_since_epoch().count() - 
        std::chrono::time_point_cast<std::chrono::microseconds>(tp1).time_since_epoch().count();
    
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    snprintf(temp, 64, "%s %d", tmp, usecs);
#else
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    snprintf(temp, 64, "%ld", std::chrono::time_point_cast<std::chrono::microseconds>(tp).time_since_epoch().count());
#endif
    return temp;
}

class LogMessage {
public:
    LogMessage() {}
    LogMessage(std::string module, int level, std::string file, int line) :
    _module(module), _level(level), _file(basename(file.c_str())), _line(line) {}

    ~LogMessage() {
	    flush();
        _data.clear();
    }
    template<class T>
    LogMessage& operator<<(const T& val) {
        if (_loging) {
           _data << " " << val;
        }
        return *this;
    }
    LogMessage& stream() {
        if (loging()) {
            _loging = true;
            //std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
            //std::chrono::high_resolution_clock::time_point tp = std::chrono::high_resolution_clock::now();
            _data << _file << ":" << _line << " ";
            _data << get_time(); // std::chrono::time_point_cast<std::chrono::microseconds>(tp).time_since_epoch().count();
            _data << " <" << _module << ">";
        } else {
            _loging = false;
        }
        return *this;
    }
    void flush() {
        if (!_loging) {
            return;
        }
        fprintf(stdout, "\033[1;3%sm", get_ansi_colorcode(level_to_color(_level)));
        std::cout << _data.str() << std::endl;
        fprintf(stdout, "\033[m");  // Resets the terminal to default.
    }
    bool loging() {
        if (!base::Module::get_instance()->get_sw(_module)) {
            return false;
        }
        int log_level = static_cast<int>(base::Module::get_instance()->get_log_level(_module));
        if (_level < log_level) {
            return false;
        }
        return true;
    }
private:
    std::stringstream _data;
    std::string _module;
    int _level;
    std::string _file;
    int _line;

    bool _loging;
};

}

#define ZLOG(module, level) \
    ZLOG_IPL_##level(module)

#define ZLOG_IPL_DEBUG(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::DEBUG), __FILE__, __LINE__).stream()

#define ZLOG_IPL_INFO(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::INFO), __FILE__, __LINE__).stream()

#define ZLOG_IPL_NOTICE(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::NOTICE), __FILE__, __LINE__).stream()

#define ZLOG_IPL_WARNING(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::WARNING), __FILE__, __LINE__).stream()

#define ZLOG_IPL_ERROR(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::ERROR), __FILE__, __LINE__).stream()

#define ZLOG_IPL_FATAL(module) \
	log::LogMessage(#module, static_cast<int>(base::LogLevel::FATAL), __FILE__, __LINE__).stream()


