#ifndef UDP_SERVER_H
#define UDP_SERVER_H


#include <algorithm>
#include <iostream>
#include <map>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>
#include <boost/asio.hpp>

#include "tcpconnection.h"




class udp_server
{
public:
    udp_server(const int& port, boost::asio::io_context& context);
    void init();

private:
    void start_accept();
    std::string m_get_name_client(const tcp::socket& klient_sock);
    void handler(TCPconnection::smart_pointer klient, const boost::system::error_code& error);

private:

    boost::asio::io_context& m_context;
    tcp::acceptor m_acceptor;
    bool is_run;

};

#endif // UDP_SERVER_H
