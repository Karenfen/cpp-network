#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H


#include <iostream>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>



class udp_client
{
public:
    udp_client(int port);
    bool init(const char* server_address);
    std::string get_last_error();
    bool run_session();

private:
    bool m_send_message(const std::string &text);
    std::pair<bool, std::string> m_recv_data();
    bool m_connected();
    const socket_wrapper::Socket& m_socket();
    bool m_server_name_is_resolved(const char *server_name);

    socket_wrapper::SocketWrapper m_sock_wrap;
    socket_wrapper::Socket m_sock{AF_INET, SOCK_STREAM, IPPROTO_TCP};
    socket_wrapper::Socket m_sock6{AF_INET6, SOCK_STREAM, IPPROTO_TCP};
    const int m_port_server;
    sockaddr_in m_addr_serv;
    sockaddr_in6 m_addr_serv6;
    sockaddr* m_addr_serv_ptr{nullptr};
    bool is_ipv4;


};

#endif // UDP_CLIENT_H
