#include "udp_client.h"
#include <cstdlib>
#include <iomanip>
#include <string>
#include <stdexcept>




udp_client::udp_client(const char* server_address, int port):
    m_port_server(port)
{
    if (!m_sock)
    {
        throw std::runtime_error(m_sock_wrap.get_last_error_string());
    }

    m_addr_serv =
    {
        .sin_family = PF_INET,
        .sin_port = htons(m_port_server),
    };

    if(!inet_pton(AF_INET, server_address, &(m_addr_serv.sin_addr)))
    {
        throw std::runtime_error("Incorrect address!");
    }

    m_addr_serv_ptr = reinterpret_cast<sockaddr*>(&m_addr_serv);

    if(!m_addr_serv_ptr)
    {
        throw std::runtime_error("Incorrect cast address!");
    }

    m_addr_serv_len = sizeof(sockaddr_in);
}


std::string udp_client::get_last_error()
{
    return m_sock_wrap.get_last_error_string();
}


bool udp_client::run_session()
{
    std::cout << "Session started..." << std::endl;

    while(true)
    {
        std::string massege{};

        std::cin >> massege;

        if(!m_send_message(massege))
            return false;

        if(massege == "exit")
            break;
    }

    std::cout << "Session ended!" << std::endl;

    return true;
}


bool udp_client::m_send_message(const std::string &text)
{
    return sendto(m_sock, (text + "\0").c_str(), (int)(text.length() + 2), 0,
                  m_addr_serv_ptr, m_addr_serv_len);
}
