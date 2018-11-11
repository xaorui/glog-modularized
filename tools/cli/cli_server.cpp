#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>   
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "cli_command.h"
#include "cli_common.h"
#include "cli_server.h"
#include <sys/prctl.h>

#include "cmd_center.h"

namespace tools {
namespace cli {

bool volatile s_is_stopping = false;

std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    //const char* convert to char*
    char* strc = new char[strlen(str.c_str()) + 1];
    memset(strc, 0, strlen(str.c_str()) + 1);
    snprintf(strc, strlen(str.c_str()) + 1, "%s", str.c_str());
    std::vector<std::string> result_vec;
    char* tmp_str = strtok(strc, pattern.c_str());

    while (tmp_str != nullptr) {
        result_vec.push_back(std::string(tmp_str));
        tmp_str = strtok(nullptr, pattern.c_str());
    }

    delete[] strc;

    return result_vec;
}

int process_client(Client &client, char *recv_buf, int len) {
    int cn = 0;
    char send_buf[BUFSIZE];
    std::string cmdline(recv_buf);
    std::vector<std::string> cmd = split(cmdline, " ");
    cn = cmd.size();
    if (cn <= 0) {
        return 0;

    } else if (cn == 1 && strcmp(cmd[0].c_str(), "quit") == 0) {
        return -1;
    } else {
        std::ostringstream out;

        if (cmd_exec(cmd, out) == false) {
            out << "***!*** "; 
            for (int i = 0; i < cn; i++) {
                out << cmd[i] << " ";
            }
            out << "exec failed!\n";
            //return 1;
        }

        std::string out_str = out.str();
        int sn = static_cast<int>(out_str.size());
        const char *psend = out_str.c_str();
        int offset = 0;
        int slen = 0;
        while (offset < sn) {
            memset(send_buf, 0, BUFSIZE);
            if (sn - offset > BUFSIZE) {
                slen = BUFSIZE;
            } else {
                slen = sn - offset;
            }
            
            memcpy(send_buf, psend + offset, slen);
            if (send(client._fd, send_buf, slen, 0) < 0) {
                std::cout << "send error" << std::endl;
                return -1;
            }
            offset += slen;
        }
    }
    return 0;
}

void cli_server_thread(volatile bool* is_stopping) {
    struct sockaddr_in server; //服务器地址
    int port = DEFAULT_PORT; //服务器端口号
    int listenfd = 0; //接收连接用描述符
    int connectfd = 0;
    int sockfd = 0;
    int rn = 0; //接收报文的字节数
    char recv_buf[BUFSIZE]; //接收缓冲区
    int sin_size = 0;
    Client client[MAX_CLINET];
    int i = 0;
    int maxfd = 0;
    int maxi = 0;
    fd_set rset;
    fd_set allset;

    cmd_init();

    std::stringstream thread_name;
    thread_name << "CLI server";
    prctl(PR_SET_NAME, thread_name.str().c_str());
    
    //使用TCP协议打开一个套接字
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return;
    }
    
    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) == -1) {
        close(listenfd);
        return;
    }
    int on=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    
    //设定服务器地址
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind");
        close(listenfd);
        return;
    }

    //开始接受建立连接请求
    if (listen(listenfd, MAX_CLINET) < 0) {
        perror("listen");
        close(listenfd);
        return;
    }
    maxfd = listenfd;  
    maxi = -1;  
    for (i = 0; i < MAX_CLINET; i++)  
    {  
        client[i]._fd = -1;  
    }  
    FD_ZERO(&allset); 
    FD_SET(listenfd, &allset);  
    //cmd_init();
    struct timeval tv;
    while (*is_stopping == false) {
        tv.tv_sec = 0;
        tv.tv_usec = 250000;
        rset = allset;
        int nready = select(maxfd + 1, &rset, nullptr, nullptr, &tv);
        if (nready < 0) {
            printf("Select error \n");
            break;
        } else if (nready == 0) {
            continue;
        } else {
            
        }
        if (FD_ISSET(listenfd, &rset)) {
            struct sockaddr_in addr;
            sin_size = sizeof(struct sockaddr_in);
            if ((connectfd = 
                accept(listenfd, (struct sockaddr *)&addr, (socklen_t *) & sin_size)) == -1) {
                perror("Accept() error\n");
                continue;
            }

            for (i = 0; i < MAX_CLINET; i++) {
                if (client[i]._fd < 0) {
                    client[i]._fd = connectfd;
                    client[i]._addr = addr;
                    printf("You got a connection from %s:%d.\n", 
                        inet_ntoa(client[i]._addr.sin_addr),ntohs(client[i]._addr.sin_port));
                    break;  
                }  
            }  
            if (i == MAX_CLINET) {
                printf("Too many clients\n");
                continue;
            }
            FD_SET(connectfd, &allset);  
            if (connectfd > maxfd)  
                maxfd = connectfd;
            if (i > maxi)
                maxi = i;  
            if (--nready <= 0)
                continue;
        }  
  
        for (i = 0; i <= maxi; i++)
        {  
            if ((sockfd = client[i]._fd) < 0) {
                continue;
            }
            if (FD_ISSET(sockfd, &rset)) {
                memset(recv_buf, 0, BUFSIZE);
                if ((rn = recv(sockfd, recv_buf, BUFSIZE, 0)) == 0) {   
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i]._fd = -1; 
                } else {
                    recv_buf[rn] = '\0';
                    if (process_client(client[i], recv_buf, rn) == -1) {
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i]._fd = -1; 
                    }
                }
                if (--nready <= 0)
                    break;
            }
        }
    }
    for (i = 0; i <= maxi; i++) {
        if ((sockfd = client[i]._fd) > 0) {
            close(sockfd);
        }
    }
    close(listenfd);
    return;
}

}
}

