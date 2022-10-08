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

    socket_wrapper::SocketWrapper m_sock_wrap;
    socket_wrapper::Socket m_sock{AF_INET, SOCK_STREAM, IPPROTO_TCP};
    const int m_port_server;
    sockaddr_in m_addr_serv;
    sockaddr* m_addr_serv_ptr{nullptr};
    socklen_t m_addr_serv_len;
    //std::string m_buffer{};
};

#endif // UDP_CLIENT_H
