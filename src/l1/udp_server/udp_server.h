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
    void connection_handling(socket_wrapper::Socket client_socket, sockaddr client_address_);

    socket_wrapper::SocketWrapper m_sock_wrap;
    socket_wrapper::Socket m_sock{AF_INET, SOCK_STREAM, IPPROTO_TCP};
    const int m_port;
    sockaddr_in m_addr;
    bool m_is_run;
    std::map<std::string, std::function<void(const socket_wrapper::Socket& sender)>> m_commands;

    // commands
    void m_exit(const socket_wrapper::Socket& sender);
    void m_shootdown(const socket_wrapper::Socket& sender);
};

#endif // UDP_SERVER_H