#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace tools {
namespace cli {

extern bool volatile s_is_stopping;
class Client{
public:
    Client(){};
    ~Client(){};
    int _fd;
    struct sockaddr_in _addr;
};
 
void cli_server_thread(volatile bool* is_stopping);

}
}
