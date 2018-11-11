#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <functional>
#include <stdint.h>
#include <typeinfo>

#include "design_pattern.h"

namespace  base {

typedef std::function<bool(std::string name, std::ostringstream& out)>  cmd_get_func;
typedef std::function<bool(std::string name, std::vector<std::string> param, std::ostringstream& out)>  cmd_set_func;

class ParameterServer {
public:

typedef std::function<bool(std::string name, std::ostringstream& out)>  cmd_base_get_cb;
typedef std::function<bool(std::string name, std::string param, std::ostringstream& out)>  cmd_base_set_cb;

    void init() {
        #define BASE_CB_SETUP(type, func) \
            _base_set_cb[#type] = std::bind(&base::ParameterServer::set_##func, this, \
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3); \
            _base_get_cb[#type] = std::bind(&base::ParameterServer::get_##func, this, \
                std::placeholders::_1, std::placeholders::_2);
        
        BASE_CB_SETUP(bool, bool)
        BASE_CB_SETUP(u8, uint8_t)
        BASE_CB_SETUP(s8, int8_t)

        BASE_CB_SETUP(u16, uint16_t)
        BASE_CB_SETUP(s16, int16_t)

        BASE_CB_SETUP(u32, uint32_t)
        BASE_CB_SETUP(s32, int16_t)

        BASE_CB_SETUP(u64, uint64_t)
        BASE_CB_SETUP(s64, int64_t)

        BASE_CB_SETUP(fl, float)
        BASE_CB_SETUP(dl, double)
        BASE_CB_SETUP(str, str)
        BASE_CB_SETUP(ch, uint8_t)

        #undef BASE_CB_SETUP
    }
    template<typename T>
    T* reg(const std::string& name, const std::string& des) {
        if(exist(name)) {
            return (T*)_data_map[name];
        } else {
            T* dat = new T;
            _data_map[name] = (intptr_t*)dat;
            _data_map_type[name] = std::string(typeid(T).name());
            _data_des[name] = des;
            //_data_get_func[name] = nullptr;
            //_data_set_func[name] = nullptr;
            bind(name);
            return dat;
        }
    }

    template<typename T>
    bool bind(std::string name, cmd_get_func g_func, cmd_set_func s_func) {
        _data_get_func[name] = g_func;
        _data_set_func[name] = s_func;
        return true;
    }
    bool bind(std::string name) {
        _data_get_func[name] = std::bind(&base::ParameterServer::base_get, this, 
            std::placeholders::_1, std::placeholders::_2);
        _data_set_func[name] = std::bind(&base::ParameterServer::base_set, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return true;
    }
    template<typename T> 
    bool save(std::string name, T val) {
        if (exist(name)) {
            //delete _data_map[name];
            //_data_map[name] = (intptr_t*) (new T(val));
            *((T*)_data_map[name]) = val;
        } else {
            T* dat = reg<T>(name, std::string("no reg and bind"));
            if (dat == nullptr) {
                return false;
            }
            *dat = val;
        }
        return true;
    }

    template<typename T> 
    T* get(const std::string& name) {
        if (exist(name)) {
            return (T*)_data_map[name];
        } else {
            return nullptr;
        }
    }

    bool base_get(std::string name, std::ostringstream& out) {
        if (!exist(name)) {
            return false;
        }
        std::string type;
        if (!get_base_tpye(name, type)) {
            return false;
        }
        auto iter = _base_get_cb.find(type);
        if (iter == _base_get_cb.end()) {
            return false;
        }

        return iter->second(name, out);
    }
    bool base_set(std::string name, std::vector<std::string> param, std::ostringstream& out) {
        if (!exist(name)) {
            return false;
        }
        if (param.size() != 1) {
            return false;
        }
        std::string type;
        if (!get_base_tpye(name, type)) {
            return false;
        }
        auto iter = _base_set_cb.find(type);
        if (iter == _base_set_cb.end()) {
            return false;
        }

        return iter->second(name, param[0], out);
    }

    bool cmd(const std::vector<std::string> cmd, std::ostringstream& out);
    bool cmd_help(std::ostringstream& out);

protected:
    bool exist(const std::string& name);
    bool get_base_tpye(std::string name, std::string& type_map);
    
    bool set_bool(std::string name, std::string param, std::ostringstream& out);
    bool set_int8_t(std::string name, std::string param, std::ostringstream& out);
    bool set_uint8_t(std::string name, std::string param, std::ostringstream& out);
    bool set_int16_t(std::string name, std::string param, std::ostringstream& out);
    bool set_uint16_t(std::string name, std::string param, std::ostringstream& out);
    bool set_int32_t(std::string name, std::string param, std::ostringstream& out);
    bool set_uint32_t(std::string name, std::string param, std::ostringstream& out);
    bool set_int64_t(std::string name, std::string param, std::ostringstream& out);
    bool set_uint64_t(std::string name, std::string param, std::ostringstream& out);

    bool set_float(std::string name, std::string param, std::ostringstream& out);
    bool set_double(std::string name, std::string param, std::ostringstream& out);
    bool set_str(std::string name, std::string param, std::ostringstream& out);

    bool get_bool(std::string name, std::ostringstream& out);
    bool get_int8_t(std::string name, std::ostringstream& out);
    bool get_uint8_t(std::string name, std::ostringstream& out);
    bool get_int16_t(std::string name, std::ostringstream& out);
    bool get_uint16_t(std::string name, std::ostringstream& out);
    bool get_int32_t(std::string name, std::ostringstream& out);
    bool get_uint32_t(std::string name, std::ostringstream& out);
    bool get_int64_t(std::string name, std::ostringstream& out);
    bool get_uint64_t(std::string name, std::ostringstream& out);

    bool get_float(std::string name, std::ostringstream& out);
    bool get_double(std::string name, std::ostringstream& out);
    bool get_str(std::string name, std::ostringstream& out);

private:
    std::map<std::string, intptr_t*> _data_map;
    std::map<std::string, std::string> _data_map_type;
    std::map<std::string, std::string> _data_des;
    std::map<std::string, cmd_get_func> _data_get_func;
    std::map<std::string, cmd_set_func> _data_set_func;

    std::map<std::string, cmd_base_set_cb> _base_set_cb;
    std::map<std::string, cmd_base_get_cb> _base_get_cb;


SINGLETON_CLASS_INIT(ParameterServer, init)

};

}
