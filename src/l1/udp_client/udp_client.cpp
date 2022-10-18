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
    if (!m_sock || !m_sock6)
    {
        throw std::runtime_error(m_sock_wrap.get_last_error_string());
    }

    m_addr_serv =
    {
        .sin_family = PF_INET,
        .sin_port = htons(m_port_server),
    };

    m_addr_serv6.sin6_family = PF_INET6;
    m_addr_serv6.sin6_port = htons(m_port_server);

    if(!m_server_name_is_resolved(server_address))
    {
        if(inet_pton(AF_INET, server_address, &(m_addr_serv.sin_addr)) > 0)
        {
            m_addr_serv_ptr = reinterpret_cast<sockaddr*>(&m_addr_serv);
            is_ipv4 = true;
        }
        else if(inet_pton(AF_INET6, server_address, &(m_addr_serv6.sin6_addr)) > 0)
        {
            m_addr_serv_ptr = reinterpret_cast<sockaddr*>(&m_addr_serv6);
            is_ipv4 = false;
        }
        else
        {
            std::cerr << "Incorrect server address/name" << std::endl;
            return false;
        }
    }

    if(!m_addr_serv_ptr)
    {
        return false;
    }

    return true;
}


std::string udp_client::get_last_error()
{
    return m_sock_wrap.get_last_error_string();
}


bool udp_client::run_session()
{
    if(connect(m_socket(), m_addr_serv_ptr, sizeof(sockaddr)) != 0)
    {
        std::cerr << "Connecting error!" << std::endl;
        return false;
    }

    std::cout << "Session started..." << std::endl;

    while(true)
    {
        std::string massege{};

        getline(std::cin, massege);

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
    close(m_socket());
    std::cout << "Session ended!" << std::endl;

    return true;
}


bool udp_client::m_send_message(const std::string &text)
{
    ssize_t result = send(m_socket(), (text + "\n").c_str(), (int)(text.length() + 1), 0);

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

    ssize_t recv_len = recv(m_socket(), buffer, sizeof(buffer), 0);

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


const socket_wrapper::Socket &udp_client::m_socket()
{
    if(is_ipv4)
        return m_sock;
    else
        return m_sock6;
}

bool udp_client::m_server_name_is_resolved(const char *server_name)
{
    addrinfo hints =
    {
        .ai_flags= AI_CANONNAME,
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = 0
    };

    addrinfo *servinfo = nullptr;
    bool status = false;

    if ((getaddrinfo(server_name, nullptr, &hints, &servinfo)) != 0)
    {
        return status;
    }

    for (auto const *serv_addr_info = servinfo; serv_addr_info != nullptr; serv_addr_info = serv_addr_info->ai_next)
    {
        if (AF_INET == serv_addr_info->ai_family)
        {
            sockaddr_in const * const sin = reinterpret_cast<const sockaddr_in* const>(serv_addr_info->ai_addr);
            m_addr_serv.sin_addr = sin->sin_addr;
            m_addr_serv_ptr = reinterpret_cast<sockaddr*>(&m_addr_serv);
            is_ipv4 = true;
            status = true;
            break;
        }
        else if (AF_INET6 == serv_addr_info->ai_family)
        {
            const sockaddr_in6* const sin = reinterpret_cast<const sockaddr_in6* const>(serv_addr_info->ai_addr);
            m_addr_serv6.sin6_addr = sin->sin6_addr;
            m_addr_serv_ptr = reinterpret_cast<sockaddr*>(&m_addr_serv6);
            is_ipv4 = false;
            status = true;
            break;
        }
    }

    freeaddrinfo(servinfo);

    return status;
}

