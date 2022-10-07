#include "udp_server.h"

#include <cstdlib>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <thread>



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
    listen(m_sock, 0);

    std::cout << "Running echo server...\n" << std::endl;
    char client_address_buf[INET_ADDRSTRLEN];
    m_is_run = true;

    while(m_is_run)
    {
        struct sockaddr client_addr = {0};

        socklen_t client_len = sizeof(client_addr);

        socket_wrapper::Socket client_sock(accept(m_sock, &client_addr, &client_len));

        std::cout << "Accepted connection" << std::endl;

        if (!client_sock)
        {
            throw std::logic_error(m_sock_wrap.get_last_error_string());
        }

        //Start a new thread for every new connection.
        std::thread client_thread(&udp_server::connection_handling, this, std::move(client_sock), std::move(client_addr));
        std::cout << "Creating client thread..." << std::endl;
        client_thread.join();
    }

    m_sock.close();
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

void udp_server::connection_handling(socket_wrapper::Socket client_socket, sockaddr client_address)
{
    sockaddr_in* client_address_in = reinterpret_cast<sockaddr_in*>(&client_address);
    socklen_t client_address_len = sizeof(sockaddr);
    ssize_t recv_len = 0;

    char recv_data_buf[1024];
    char client_address_buf[INET_ADDRSTRLEN] {"***************"};
    std::string name_client = get_name_client(&client_address, client_address_len);
    uint16_t client_port = ntohs(client_address_in->sin_port);
    inet_ntop(AF_INET, &client_address_in->sin_addr, client_address_buf, INET_ADDRSTRLEN);



    while (m_is_run)
        {
            // Read content into buffer from client.
            recv_len = recv(client_socket, recv_data_buf, sizeof(recv_data_buf), 0);

            if (recv_len > 0)
            {
                recv_data_buf[recv_len] = '\0';
                std::cout
                    << "Client: "
                    << name_client
                    << " with address "
                    << client_address_buf
                    << ":" << client_port
                    << " sent datagram "
                    << "[length = "
                    << recv_len
                    << "]:\n'''\n"
                    << recv_data_buf
                    << "\n'''"
                    << std::endl;

                std::string command_string = {recv_data_buf, 0, (size_t)recv_len - 1};
                rtrim(command_string);

                if(m_commands.contains(command_string))
                {
                    auto command = m_commands.at(command_string);
                    command();
                }

                // Send same content back to the client ("echo").
                send(client_socket, recv_data_buf, recv_len, 0);
            }
            else
            {
                client_socket.close();
                break;
            }

            std::cout << std::endl;
        }
}


void udp_server::m_exit()
{
    m_is_run = false;
    std::cout << "Clothing server..." << std::endl;
    m_sock.close();
}








