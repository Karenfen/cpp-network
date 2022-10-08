#include "udp_client.h"
#include <cstdlib>
#include <iomanip>
#include <string>
#include <stdexcept>




udp_client::udp_client(int port):
    m_port_server(port)
{

}

bool udp_client::init(const char *server_address)
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
        return false;
    }

    m_addr_serv_len = sizeof(sockaddr);

    return true;
}


std::string udp_client::get_last_error()
{
    return m_sock_wrap.get_last_error_string();
}


bool udp_client::run_session()
{
    if(connect(m_sock, m_addr_serv_ptr, sizeof(sockaddr)) != 0)
    {
        std::cerr << "Connecting error!" << std::endl;
        return false;
    }

    std::cout << "Session started..." << std::endl;

    while(true)
    {
        std::string massege{};

        std::cin >> massege;

        if(!m_send_message(massege))
        {
            std::cout << "Сonnection aborted!" << std::endl;
            break;
        }

        auto resv_result = m_recv_data();

        std::cout << resv_result.second << std::endl;

        if(resv_result.first == false)
            break;
    }
    m_sock.close();
    std::cout << "Session ended!" << std::endl;

    return true;
}


bool udp_client::m_send_message(const std::string &text)
{
    ssize_t result = send(m_sock, (text + "\n").c_str(), (int)(text.length() + 1), 0);

    if(result == -1)
        throw std::logic_error(get_last_error());
    else if(result == 0)
        return false;
    else
        return true;
}

std::pair<bool, std::string>  udp_client::m_recv_data()
{
    char buffer[256]{};
    std::string str_result{};
    bool result{};

    ssize_t recv_len = recv(m_sock, buffer, sizeof(buffer), 0);

    if(recv_len == -1)
        throw std::logic_error(get_last_error());
    else if(recv_len == 0)
    {
        result = false;
        str_result = "Сonnection aborted!";
    }
    else
    {
        str_result.append(buffer, recv_len);
        result = true;
    }

    return std::make_pair(result, str_result);
}

