#include "module.h"
#include "mlog_wrapper.h"
#include "parameter_server.h"

#include <pthread.h>
#include <unistd.h>
#include <thread>
#include <functional>
#include <string>
#include <iomanip>

class Data{
public:
    Data() {}
    Data(int a, double b) : _a(a), _b(b) {}
    ~Data() {}
    bool cmd_get(std::string name, std::ostringstream& out) {
        out << " a : " << _a << " " << " b : " << _b << "\n";
        return true;
    }
    bool cmd_set(std::string name, std::vector<std::string> param, std::ostringstream& out) {
        return false;
    }
private:
    int _a;
    double _b;
};

void test_typeid() {
    std::cout << "show base date type" << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "bool" << std::setw(4) << typeid(bool).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "uint8_t" << std::setw(4) << typeid(uint8_t).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "int8_t" << std::setw(4) << typeid(int8_t).name() << std::endl;

    std::cout << std::setfill(' ') << std::setw(12) << "uint16_t" << std::setw(4) << typeid(uint16_t).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "int16_t" << std::setw(4) << typeid(int16_t).name() << std::endl;

    std::cout << std::setfill(' ') << std::setw(12) << "uint32_t" << std::setw(4) << typeid(uint32_t).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "int32_t" << std::setw(4) << typeid(int32_t).name() << std::endl;

    std::cout << std::setfill(' ') << std::setw(12) << "uint64_t" << std::setw(4) << typeid(uint64_t).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "int64_t" << std::setw(4) << typeid(int64_t).name() << std::endl;

    std::cout << std::setfill(' ') << std::setw(12) << "double" << std::setw(4) << typeid(double).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "float" << std::setw(4) << typeid(float).name() << std::endl;
    std::cout << std::setfill(' ') << std::setw(12) << "string" << std::setw(4) << typeid(std::string).name() << std::endl;
}
void test_param() {
    base::ParameterServer::instance().reg<bool>(std::string("bool"), std::string("bool test"));
    base::ParameterServer::instance().reg<uint8_t>(std::string("u8"), std::string("u8 test"));
    base::ParameterServer::instance().reg<int8_t>(std::string("s8"), std::string("s8 test"));

    base::ParameterServer::instance().reg<uint16_t>(std::string("u16"), std::string("u16 test"));
    base::ParameterServer::instance().reg<int16_t>(std::string("s16"), std::string("s16 test"));

    base::ParameterServer::instance().reg<uint32_t>(std::string("u32"), std::string("u32 test"));
    base::ParameterServer::instance().reg<int32_t>(std::string("s32"), std::string("s32 test"));

    base::ParameterServer::instance().reg<uint64_t>(std::string("u64"), std::string("u64 test"));
    base::ParameterServer::instance().reg<int64_t>(std::string("s64"), std::string("s64 test"));

    base::ParameterServer::instance().reg<float>(std::string("fl"), std::string("fl test"));
    base::ParameterServer::instance().reg<double>(std::string("dl"), std::string("dl test"));
    base::ParameterServer::instance().reg<std::string>(std::string("str"), std::string("string test"));


     //Data dat(1, 2);
    Data* dat = base::ParameterServer::instance().reg<Data>(std::string("d1"), std::string("d1 test"));
    base::ParameterServer::instance().bind<Data>(std::string("d1"),
        std::bind(&Data::cmd_get, dat, std::placeholders::_1, std::placeholders::_2), 
        std::bind(&Data::cmd_set, dat, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    Data da(1, 2);
    base::ParameterServer::instance().save<Data>(std::string("d1"), da);
    int d2 = 2;
    base::ParameterServer::instance().reg<int>(std::string("d2"), std::string("d2 test"));
    
    base::ParameterServer::instance().save(std::string("d2"), d2);
    int* d3 = base::ParameterServer::instance().get<int>(std::string("d2"));
    std::cout << "get d3: " << *d3 << std::endl;

}
int main(int argc, char* argv[]) {
    //tools::cli::cmd_setup("echo", tools::cli::echo_test, "echo testing");
    LogHelper gh(argv[0]);

    base::Module::get_instance()->init();
    tools::cli::cmd_init();
    std::thread cli_server(tools::cli::cli_server_thread, &tools::cli::s_is_stopping);
    base::Module::get_instance()->regist("t1");

    test_typeid();
    test_param();
    while (1) {
#if 0
        LOG(DEBUG) << "test 0 debug";
        LOG(INFO) << "test 0 info";
        LOG(WARNING) << "test 0 warning";
        LOG(ERROR) << "test 0 error";
        //LOG(FATAL) << "test 0 fatal";

        MLOG(t1, DEBUG) << "test 1 debug";
        MLOG(t1, INFO) << "test 1 info";
        MLOG(t1, WARNING) << "test 1 warning";
        MLOG(t1, ERROR) << "test 1 error";
        MDCHECK_EQ(t1, 1, 1);
        //MLOG(t1, FATAL) << "test 1 fatal";

        MLOG(t2, INFO) << "test 2 info";
        MLOG(t2, WARNING) << "test 2 warning";
        MLOG(t2, ERROR) << "test 2 error";
#else
        ZLOG(t1, DEBUG) << "test 1 debug";
        ZLOG(t1, INFO) << "test 1 info";
        ZLOG(t1, WARNING) << "test 1 warning";
        ZLOG(t1, ERROR) << "test 1 error";
        //ZLOG(t1, FATAL) << "test 1 fatal";

        ZLOG(t2, INFO) << "test 2 info";
        ZLOG(t2, WARNING) << "test 2 warning";
        ZLOG(t2, ERROR) << "test 2 error";
        
        //MLOG(t2, FATAL) << "test 2 fatal";
        //std::this_thread::sleep_for(std::chrono::microseconds(1000));
#endif
    }
    cli_server.join();
    return 0;
}