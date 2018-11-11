#include "parameter_server.h"
#include <sstream>
#include <iomanip>

namespace base {

bool ParameterServer::exist(const std::string& name) {
    auto iter = _data_map.find(name);
    if (iter == _data_map.end()) {
        return false;
    } else {
        return true;
    }
}
bool ParameterServer::get_base_tpye(std::string name, std::string& type_map) {
    std::string type = _data_map_type[name];
    
    if (typeid(bool).name() == type) {
        type_map ="bool";
    } if (typeid(uint8_t).name() == type) {
        type_map = "u8"; 
    } else if (typeid(int8_t).name() == type) {
        type_map = "s8"; 
    } else if (typeid(uint16_t).name() == type) {
        type_map = "u16"; 
    } else if (typeid(int16_t).name() == type) {
        type_map = "s16"; 
    } else if (typeid(uint32_t).name() == type) {
        type_map = "u32"; 
    } else if (typeid(int32_t).name() == type) {
        type_map = "s32"; 
    } else if (typeid(uint64_t).name() == type) {
        type_map = "u64"; 
    } else if (typeid(int64_t).name() == type) {
        type_map = "s64"; 
    } else if (typeid(double).name() == type) {
        type_map = "dl"; 
    } else if (typeid(float).name() == type) {
        type_map = "fl"; 
    } else if (typeid(std::string).name() == type) {
        type_map = "str"; 
    } else {
        type_map = "";
        return false;
    }
    return true;
}

bool ParameterServer::cmd(const std::vector<std::string> cmd, std::ostringstream& out) {
    if (cmd.size() == 2) {
        if (cmd[1] == "show") {
            std::string type;
            out << std::setfill(' ') << std::setw(8) << "name"
                << std::setfill(' ') << std::setw(12) << "type"
                <<"\t" << "des" << "\n";
            for (auto& des : _data_des) {
                if (get_base_tpye(des.first, type)) {
                    out << std::setfill(' ') << std::setw(8) << des.first
                    << std::setfill(' ') << std::setw(12) << type 
                    <<"\t"  << des.second << "\n";
                } else {
                    out << std::setfill(' ') << std::setw(8) << des.first 
                    << std::setfill(' ') << std::setw(12) << _data_map_type[des.first] 
                    <<"\t"  << des.second << "\n";
                }
            }
            return true;
        }
    } else if (cmd.size() == 3) {
        if (cmd[1] == "get") {
            std::string name = cmd[2];
            if (!exist(name)) {
                out << " param " << name << " not exist" << "\n";
                return false;
            }
            auto iter = _data_get_func.find(name);
            if (iter != _data_get_func.end()
                && iter->second != nullptr) {
                return iter->second(name, out);
            }
        } else {
            return false;
        }
    } else if (cmd.size() >= 4) {
        if (cmd[1] == "set") {
            std::string name = cmd[2];
            if (!exist(name)) {
                out << " param " << name << " not exist" << "\n";
                return false;
            }
            std::vector<std::string> param;
            param.assign(cmd.begin() + 3, cmd.end());
            auto iter = _data_set_func.find(name);
            if (iter != _data_set_func.end()
                && iter->second != nullptr) {
                return iter->second(name, param, out);
            }
        }
    }
    return false;
}

bool ParameterServer::cmd_help(std::ostringstream& out) {
    out << "\tshow" << "\t" << "\tshow all registed param des\n";
    out << "\tget" << "\tname" << "\tget registed param\n";
    out << "\tset" << "\tname" << "\tset registed param\n";
    return true;
}

#define PARAM_SET_GET(type, temp_type, trans, min, max) \
    PARAM_SET(type, temp_type, trans, min, max) \
    PARAM_GET(type)

#define PARAM_SET(type, temp_type, trans, min, max) \
bool ParameterServer::set_##type(std::string name, std::string param, std::ostringstream& out) { \
    temp_type temp = trans(param.c_str()); \
    if (temp < min || temp > max) { \
        out << " out of range for " << #type << "\n"; \
        return false; \
    } \
    *((type*)_data_map[name]) = temp; \
    return true; \
}

#define PARAM_GET(type) \
bool ParameterServer::get_##type(std::string name, std::ostringstream& out) { \
    out << " " << name << " : " << *((type*)_data_map[name]) << "\n"; \
    return true; \
}

PARAM_SET_GET(int8_t, int32_t, atoi, INT8_MIN, INT8_MAX)
PARAM_SET_GET(uint8_t, int32_t, atoi, 0, UINT8_MAX)

PARAM_SET_GET(int16_t, int32_t, atoi, INT16_MIN, INT16_MAX)
PARAM_SET_GET(uint16_t, int32_t, atoi, 0, UINT16_MAX)

PARAM_SET_GET(int32_t, int32_t, atoi, INT32_MIN, INT32_MAX)
PARAM_SET_GET(uint32_t, int64_t, atoll, 0, UINT32_MAX)

PARAM_SET_GET(int64_t, int64_t, atoll, INT64_MIN, INT64_MAX)
PARAM_SET_GET(uint64_t, uint64_t, atoll, 0, UINT64_MAX)

#undef PARAM_SET_GET
#undef PARAM_SET

PARAM_GET(bool)
PARAM_GET(float)
PARAM_GET(double)

#undef PARAM_GET

bool ParameterServer::set_bool(std::string name, std::string param, std::ostringstream& out) {
    bool result = false;
    if (param == "true") {
        result = true;
    } else if (param == "false") {
        result = false;
    } else {
        return false;
    }
    *((bool*)_data_map[name]) = result;
    return true;
}

bool ParameterServer::set_float(std::string name, std::string param, std::ostringstream& out) {
    float result = atof(param.c_str()); 
    *((float*)_data_map[name]) = result;
    return true;
}

bool ParameterServer::set_double(std::string name, std::string param, std::ostringstream& out) {
    double result = atof(param.c_str()); 
    *((double*)_data_map[name]) = result;
    return true;
}

bool ParameterServer::set_str(std::string name, std::string param, std::ostringstream& out) {
    *((std::string*)_data_map[name]) = param;
    return true;
}
bool ParameterServer::get_str(std::string name, std::ostringstream& out) {
    out << " " << name << " : " << *((std::string*)_data_map[name]) << "\n"; 
    return true;
}

}
