#pragma once

#define SINGLETON_CLASS(classname) \
public: \
    static classname& instance() { \
        static classname s_instance; \
        return s_instance; \
    } \
private: \
    classname() {} \
    ~classname() {} 

#define SINGLETON_CLASS_INIT(classname, init) \
public: \
    static classname& instance() { \
        static classname s_instance; \
        return s_instance; \
    } \
private: \
    classname() { init();} \
    ~classname() {} 
