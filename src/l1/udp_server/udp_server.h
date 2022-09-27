#ifndef UDP_SERVER_H
#define UDP_SERVER_H


#include <algorithm>
#include <iostream>
#include <map>
#include <functional>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base());
    return s;
}



class udp_server
{
public:
    udp_server(const int& port);
    bool start();
    void run();
    std::string get_last_error();

private:
    std::string get_name_client(sockaddr *client_address, socklen_t client_address_len);

    socket_wrapper::SocketWrapper m_sock_wrap;
    socket_wrapper::Socket m_sock{AF_INET, SOCK_DGRAM, IPPROTO_UDP};
    const int m_port;
    sockaddr_in m_addr;
    char m_buffer[256];
    bool m_is_run;
    std::map<std::string, std::function<void()>> m_commands;

    // commands
    void m_exit();
};

#endif // UDP_SERVER_H
