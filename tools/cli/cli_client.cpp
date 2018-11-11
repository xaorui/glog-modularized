#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
#include <chrono>
#include <thread>
#include <termios.h>
#include "cli_common.h"

int g_running = 1;
const int sleep_time_us = 1000;
const char* cmd_prompt = "<#>";
pthread_t cli_tid;

struct cli_cmd {
    char cmd[256];
    int cmd_len;
    int loop_num;
    double loop_delay; // s
};

void cmd_support_options() {
    printf("--------------------------------------\n");
    printf(" Support options:\n");
    printf("  -d num cmd loop delay unit:s\n");
    printf("  -n num cmd loop num\n");
    printf("--------------------------------------\n");
}

int cmd_parse(char* cmd_str, struct cli_cmd& cmd) {
    //char test[] = "./dag_hz data_name -d   1  -s   -n 10";
    char* p = nullptr;
    char split[] = "-";
    char split2[] = " ";
    char tmp[10][128] = {0};
    int cmd_num = 0;

    p = strtok(cmd_str, split);
    if (p == nullptr) {
        return 0;
    }

    cmd.cmd_len = strlen(p);
    if (cmd.cmd_len > 256) {
        printf("cmd is too long\n");
        return 1;
    }
    memcpy(cmd.cmd, p, cmd.cmd_len);

    while ((p = strtok(nullptr, split)) != nullptr) {
        snprintf(tmp[cmd_num++], 256, "%s", p);
        if (cmd_num > 10) {
            printf("cmd is too long only support 10 options\n");
            return 1;
        }
    }

    for (int i = 0; i < cmd_num; i++) {
        p = strtok(tmp[i], split2);

        while (p != nullptr) {
            switch (*p) {
            case 'd':
                p = strtok(nullptr, split2);
                if (p != nullptr) {
                    cmd.loop_delay = atof(p);
                }
                break;

            case 'n':
                p = strtok(nullptr, split2);
                if (p != nullptr) {
                    cmd.loop_num = atoi(p);
                }
                break;

            default:
                p = strtok(nullptr, split2);
                printf("not support option\n");
                return -1;
            }

            p = strtok(nullptr, split2);
            if (p != nullptr) {
                printf("cmd string error\n");
                return -1;
            }
        }
    }

    return 0;
}

int cmd_transfer(int fd, struct cli_cmd cmd) {
    double loop_delay = 0;
    int loop_num = 0;

    if (cmd.loop_num == 0) {
        send(fd, cmd.cmd, cmd.cmd_len, 0);
        return 0;
    }

    loop_num = cmd.loop_num;

    if (cmd.loop_delay == 0) {
        loop_delay = 1000000;
    } else {
        loop_delay = cmd.loop_delay * 1000000;
    }

    while (loop_num--) {
        //printf("\n------------------------------------------------\n");
        send(fd, cmd.cmd, cmd.cmd_len, 0);
        usleep(loop_delay);
    }
    return 0;
}
void* cli_command(void* arg) {
    int sockfd = (int)(*(int*)(arg));
    int n = 0;
    char cmd[tools::cli::BUFSIZE];    //发送缓
    struct cli_cmd cmdparse;
    int ret = 0;

    while (g_running) {
        /*读取命令*/
        char *input = readline(cmd_prompt);
        if (!input) {
            continue;
        }
        if (!g_running) {
            break;
        }
        if (*input) {
            add_history(input);
        }
        if (strcmp(input, "list") == 0) {
            HIST_ENTRY **list;
            register int i;
            list = history_list();
            if (list) {
                for (i = 0; list[i]; i++) {
                    fprintf(stdout, "%d: %s\r\n", i, list[i]->line);
                }
                free(input);
                continue;
            }
        }
        memset(cmd, 0, tools::cli::BUFSIZE);
        memcpy(cmd, input, strlen(input));
        free(input);

        n = strlen(cmd);

        if (strncmp(cmd, "quit", 4) == 0) {
            g_running = 0;
            break;
        }

        memset(&cmdparse, 0, sizeof(struct cli_cmd));
        ret = cmd_parse(cmd, cmdparse);

        if (ret == 0) {
            cmd_transfer(sockfd, cmdparse);
        } else if (ret == -1) {
            if ((n = send(sockfd, "help", strlen("help"), 0)) <= 0) {
                continue;
            }

            cmd_support_options();
        } else {
            if ((n = send(sockfd, "help", strlen("help"), 0)) <= 0) {
                continue;
            }
        }
        usleep(sleep_time_us);
    }
    memset(cmd, 0, tools::cli::BUFSIZE);
    strcpy(cmd, "quit");
    n = strlen(cmd);
    send(sockfd, cmd, n, 0);
    return nullptr;
}
void stop(int signo) {
    fflush(stdout);
    //printf("Please use quit to quit\n");
    g_running = 0;
    pthread_kill(cli_tid, SIGQUIT);
}

int main(int argc, char* argv[]) {
    struct sockaddr_in server;    //服务器地址
    unsigned long dst_ip = 0;         //服务器IP地址
    int port = tools::cli::DEFAULT_PORT;     //端口号
    int s = 0;        //套接字的描述符
    int n = 0;        //输入数据的字节数
    char buf[tools::cli::BUFSIZE];    //接收缓
    struct timeval tv;    //select超时时间
    fd_set readfd;        //使用select检索出的描述符

    //实际参数的检查
    if (argc !=1 && argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s [ip] [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    std::string ip;
    if (argc == 1) {
        ip = "127.0.0.1";
    } else {
        ip = argv[1];
    }

    //检索服务器的ip地址
    if ((dst_ip = inet_addr(ip.c_str())) == INADDR_NONE) {
        struct hostent* he;  //主机信息

        if ((he = gethostbyname(argv[1])) == nullptr) {
            fprintf(stderr, "gethostbyname error\n");
            exit(EXIT_FAILURE);
        }

        memcpy((char*)&dst_ip, (char*)he->h_addr, strlen(he->h_addr));
    }

    //检索服务器的端口号
    if (argc == 3) {
        if ((port = atoi(argv[2])) == 0) {
            struct servent* se;  //服务信息

            if ((se = getservbyname(argv[2], "tcp")) != nullptr) {
                port = (int) ntohs((u_short)se->s_port);
            } else {
                fprintf(stderr, "getservbyname error\n");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        port = tools::cli::DEFAULT_PORT;
    }



    //使用TCP协议打开一个套接字
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //设定服务器的地址，建立一个连接
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = dst_ip;
    server.sin_port = htons(port);

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Connected to '%s'\n", inet_ntoa(server.sin_addr));
    struct termios term;
    if( tcgetattr(STDIN_FILENO, &term) < 0 ) {
        perror("tcgetattr");
        exit(1);
    }
    //pthread_t cli_tid;
    pthread_create(&cli_tid, nullptr, cli_command, &s);

    signal(SIGINT,stop); /*注册SIGINT 信号*/
    //客户机处理的主要子程序
    while (g_running) {
        //select超时的设定
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        //标准输入，有无来自服务器的报文
        FD_ZERO(&readfd);
        FD_SET(s, &readfd);

        if ((select(s + 1, &readfd, nullptr, nullptr, &tv)) <= 0) {
            continue;
        }

        //服务器
        if (FD_ISSET(s, &readfd)) {
            memset(buf, 0, tools::cli::BUFSIZE);

            if ((n = recv(s, buf, tools::cli::BUFSIZE - 1, 0)) <= 0) {
                g_running = 0;
                //pthread_join(cli_tid, nullptr);
                fprintf(stderr, "connection closed.\n");
                break;
            }

            buf[n] = '\0';
            printf("%s", buf);
            fflush(stdout);
        }
    }
    //pthread_kill(cli_tid, SIGQUIT);
    //pthread_join(cli_tid, nullptr);
    fflush(stdout);
    close(s);
    if( tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0 ) {
        perror("tcsetattr");
        exit(1);
    }

    return EXIT_SUCCESS;
}

