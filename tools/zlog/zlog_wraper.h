#pragma one

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#include "module.h"
#include "zlog.h"

class LogMessage {
public:
    LogMessage() {
        _ctr.clear();
        _ctr << "Time format";
    }
    ~LogMessage() {}
    std::string& ctr() {
        return _ctr.str()；
    }
private:
    std::stringstream _ctr;
};

class Zlog : public std::ostream {
public:
    Zlog() {}
    ~Zlog() {}
    std::ostream& operator<<(std::ostream& os) {
        return os;
    }
    static std::ostream& stream() {
        LogMessage log;
        return (this->out() << log.ctr());
    }
    static std::ostream& stream(std::module) {
        LogMessage log;
        return (this->out() << log.ctr());
    }
private:

}

#define COMPACT_GOOGLE_LOG_DEBUG @ac_google_namespace@::LogMessage( \
      __FILE__, __LINE__, @ac_google_namespace@::GLOG_DEBUG)


ostream& operator<<(ostream &os, const PRIVATE_Counter&) {
#ifdef DISABLE_RTTI
  LogMessage::LogStream *log = static_cast<LogMessage::LogStream*>(&os);
#else
  LogMessage::LogStream *log = dynamic_cast<LogMessage::LogStream*>(&os);
#endif
  CHECK(log && log == log->self())
      << "You must not use COUNTER with non-glog ostream";
  os << log->ctr();
  return os;
}

zlog_category_t *log_cat = nullptr;
#define ZLOG_INIT(config) \
    do { \
        int rc; \
        rc = zlog_init(config); \
        if (rc) { \
            break; \
        } \
        log_cat = zlog_get_category("log_cat"); \
        if (!log_cat) { \
            zlog_fini(); \
            break; \
        } \
    } while(0)

#define zlog_DEBUG zlog_debug
#define zlog_INFO zlog_info
#define zlog_NOTICE zlog_notice
#define zlog_WARNING zlog_warning
#define zlog_ERROR zlog_error
#define zlog_FATAL zlog_fatal

#define ZLOG(level, module, args...) \
    do { \
        if (base::Module::get_instance()->get_sw(module) == false) { \
            break; \
        } \
        base::LogLevel lev = base::Module::get_instance()->get_level(#level); \
        if (base::Module::get_instance()->get_log_level(module) > lev) { \
            break; \
        } \
        std::ostringstream ss; \
        log::print(ss, args); \
        zlog_##level(log_cat, "%s", ss.str().c_str()); \
    } while(0)

#define zlog_DEBUG zlog_debug
#define zlog_INFO zlog_info
#define zlog_NOTICE zlog_notice
#define zlog_WARNING zlog_warning
#define zlog_ERROR zlog_error
#define zlog_FATAL zlog_fatal

#endif 