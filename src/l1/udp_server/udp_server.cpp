#include "udp_server.h"

#include <cstdlib>
#include <iomanip>
#include <string>
#include <stdexcept>



udp_server::udp_server(const int& port):
    m_port(port)
{

    if (!m_sock)
    {
        throw std::runtime_error(m_sock_wrap.get_last_error_string());
    }

    m_addr =
    {
        .sin_family = PF_INET,
        .sin_port = htons(m_port),
    };

    m_addr.sin_addr.s_addr = INADDR_ANY;

    m_commands["exit"] = [this](){this->m_exit();};
}


bool udp_server::start()
{
    std::cout << "Starting echo server on the port " << m_port << "...\n";

    if (bind(m_sock, reinterpret_cast<const sockaddr*>(&m_addr), sizeof(m_addr)) != 0)
    {
        std::cerr << m_sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return false;
    }

    return  true;
}


void udp_server::run()
{
    // socket address used to store client address
    struct sockaddr_in client_address = {0};
    socklen_t client_address_len = sizeof(sockaddr_in);
    ssize_t recv_len = 0;

    std::cout << "Running echo server...\n" << std::endl;
    char client_address_buf[INET_ADDRSTRLEN];
    m_is_run = true;

    while (m_is_run)
    {
        // Read content into buffer from an incoming client.
        recv_len = recvfrom(m_sock, m_buffer, sizeof(m_buffer) - 1, 0,
                            reinterpret_cast<sockaddr *>(&client_address),
                            &client_address_len);

        if (recv_len > 0)
        {
            m_buffer[recv_len] = '\0';
            std::cout
                << "Client: "
                << get_name_client(reinterpret_cast<sockaddr *>(&client_address), client_address_len)
                << " with address "
                << inet_ntop(AF_INET, &client_address.sin_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
                << ":" << ntohs(client_address.sin_port)
                << " sent datagram "
                << "[length = "
                << recv_len
                << "]:\n'''\n"
                << m_buffer
                << "\n'''"
                << std::endl;

            std::string command_string = {m_buffer, 0, (size_t)recv_len - 1};
            rtrim(command_string);

            if(m_commands.contains(command_string))
            {
                auto command = m_commands.at(command_string);
                command();
            }

            // Send same content back to the client ("echo").
            sendto(m_sock, m_buffer, recv_len, 0, reinterpret_cast<const sockaddr *>(&client_address),
                   client_address_len);
        }

        std::cout << std::endl;
    }
}


std::string udp_server::get_last_error()
{
    return m_sock_wrap.get_last_error_string();
}


std::string udp_server::get_name_client(sockaddr *client_address, socklen_t client_address_len)
{
    const socklen_t client_name_len = NI_MAXHOST;
    char client_name_buf[client_name_len];

    if(!getnameinfo(client_address, client_address_len, client_name_buf, client_name_len, NULL, 0, NI_NAMEREQD))
        return client_name_buf;

    return "*****";
}


void udp_server::m_exit()
{
    m_is_run = false;
    std::cout << "Clothing server..." << std::endl;
    m_sock.close();
}








