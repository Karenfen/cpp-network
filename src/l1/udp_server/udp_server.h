#ifndef UDP_SERVER_H
#define UDP_SERVER_H


#include <algorithm>
#include <iostream>
#include <map>
#include <functional>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

#define IS_COMMAND_SIZE(x) ((x) <= 10)
#define SIZE_BUFFER 1024

// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base());
    return s;
}



class udp_server
{
public:
    udp_server(const int& port, const int& port6);
    bool init();
    bool start();
    void run();
    std::string get_last_error();

private:
    std::string m_get_name_client(sockaddr *client_address, socklen_t client_address_len);
    void m_connection_handling(socket_wrapper::Socket client_socket, sockaddr client_address);
    void m_loop(socket_wrapper::Socket serv_sock);

    socket_wrapper::SocketWrapper m_sock_wrap;
    socket_wrapper::Socket m_sock{AF_INET, SOCK_STREAM, IPPROTO_TCP};
    socket_wrapper::Socket m_sock6{AF_INET6, SOCK_STREAM, IPPROTO_TCP};
    const int m_port;
    const int m_port6;
    sockaddr_in m_addr;
    sockaddr_in6 m_addr6;
    bool m_is_run;
    std::map<std::string, std::function<void(const socket_wrapper::Socket& sender)>> m_commands;

    // commands
    void m_exit(const socket_wrapper::Socket& sender);
    void m_shutdown(const socket_wrapper::Socket& sender);
};

#endif // UDP_SERVER_H
