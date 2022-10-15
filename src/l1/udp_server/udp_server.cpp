#include "udp_server.h"

#include <cstdlib>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <thread>



udp_server::udp_server(const int& port):
    m_port(port)
{


}

bool udp_server::init()
{
    if (!m_sock || !m_sock6)
    {
        throw std::runtime_error(m_sock_wrap.get_last_error_string());
    }

    m_addr =
    {
        .sin_family = PF_INET,
        .sin_port = htons(m_port),
    };


    m_addr.sin_addr.s_addr = INADDR_ANY;

    m_addr6 =
    {
      .sin6_family = PF_INET6,
      .sin6_port = htons(m_port),
      .sin6_addr = in6addr_any
    };

    const int flag = 1;

    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag), sizeof(flag));
    setsockopt(m_sock6, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag), sizeof(flag));



    m_commands["exit"] = [this](const socket_wrapper::Socket& sender){this->m_exit(sender);};
    m_commands["shutdown"] = [this](const socket_wrapper::Socket& sender){this->m_shutdown(sender);};

    return true;
}


bool udp_server::start()
{
    std::cout << "Starting echo server ip4 on the port " << m_port << "...\n";

    if (bind(m_sock, reinterpret_cast<const sockaddr*>(&m_addr), sizeof(m_addr)) != 0)
    {
        std::cerr << m_sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return false;
    }

    std::cout << "Starting echo server ip6 on the port " << m_port << "...\n";

    if (bind(m_sock6, reinterpret_cast<const sockaddr*>(&m_addr6), sizeof(m_addr6)) != 0)
    {
        std::cerr << m_sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return false;
    }

    return  true;
}


void udp_server::run()
{
    listen(m_sock, 10);
    listen(m_sock6, 10);
    m_is_run = true;


    std::cout << "Running echo server ip4...\n" << std::endl;
    std::cout << "Running echo server ip6...\n" << std::endl;

    std::thread loop_thread(&udp_server::m_loop, this, std::move(m_sock));
    std::thread loop_thread6(&udp_server::m_loop, this, std::move(m_sock6));

    loop_thread.join();
    loop_thread6.join();
}


std::string udp_server::get_last_error()
{
    return m_sock_wrap.get_last_error_string();
}


std::string udp_server::m_get_name_client(sockaddr *client_address, socklen_t client_address_len)
{
    const socklen_t client_name_len = NI_MAXHOST;
    char client_name_buf[client_name_len];

    if(!getnameinfo(client_address, client_address_len, client_name_buf, client_name_len, NULL, 0, NI_NAMEREQD))
        return client_name_buf;

    return "*****";
}

void udp_server::m_connection_handling(socket_wrapper::Socket client_socket, sockaddr client_address)
{
    socklen_t client_address_len = sizeof(sockaddr);
    ssize_t recv_len = 0;
    char recv_data_buf[SIZE_BUFFER];
    std::string name_client = m_get_name_client(&client_address, client_address_len);
    uint16_t client_port;
    std::string client_address_str{};


    if(client_address.sa_family == AF_INET)
    {
        sockaddr_in* client_address_in = reinterpret_cast<sockaddr_in*>(&client_address);

        char client_address_buf[INET_ADDRSTRLEN] {"***************"};

        client_port = ntohs(client_address_in->sin_port);
        inet_ntop(AF_INET, &client_address_in->sin_addr, client_address_buf, INET_ADDRSTRLEN);
        client_address_str = client_address_buf;
    }
    else if(client_address.sa_family == AF_INET6)
    {
        sockaddr_in6* client_address_in = reinterpret_cast<sockaddr_in6*>(&client_address);

        char client_address_buf[INET6_ADDRSTRLEN] {"*********************************************"};
        client_port = ntohs(client_address_in->sin6_port);
        inet_ntop(AF_INET6, &client_address_in->sin6_addr, client_address_buf, INET6_ADDRSTRLEN);
        client_address_str = client_address_buf;
    }
    else
    {
        client_socket.close();

        throw std::logic_error("Unknown address family!");
    }



    while (m_is_run)
        {
            // Read content into buffer from client.
            recv_len = recv(client_socket, recv_data_buf, SIZE_BUFFER, 0);

            if (recv_len > 0)
            {
                recv_data_buf[recv_len] = '\0';
                std::cout
                    << "Client: "
                    << name_client
                    << " with address "
                    << client_address_str
                    << ":" << client_port
                    << " sent datagram "
                    << "[length = "
                    << recv_len
                    << "]:\n'''\n"
                    << recv_data_buf
                    << "\n'''"
                    << std::endl;

                std::string command_string = {recv_data_buf, (size_t)recv_len};
                rtrim(command_string);

                if(IS_COMMAND_SIZE(command_string.length()))
                {
                    if(m_commands.contains(command_string))
                    {
                        auto command = m_commands.at(command_string);
                        command(client_socket);
                    }
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

void udp_server::m_loop(socket_wrapper::Socket serv_sock)
{
    while(m_is_run)
    {
        struct sockaddr client_addr = {0};

        socklen_t client_len = sizeof(client_addr);

        socket_wrapper::Socket client_sock(accept(serv_sock, &client_addr, &client_len));

        std::cout << "Accepted connection" << std::endl;

        if (!client_sock)
        {
            throw std::logic_error(m_sock_wrap.get_last_error_string());
        }

        //Start a new thread for every new connection.
        std::thread client_thread(&udp_server::m_connection_handling, this, std::move(client_sock), std::move(client_addr));
        std::cout << "Creating client thread..." << std::endl;
        client_thread.join();
    }

    serv_sock.close();
}


void udp_server::m_exit(const socket_wrapper::Socket& sender)
{
    close(sender);
}

void udp_server::m_shutdown(const socket_wrapper::Socket &sender)
{
    close(sender);
    m_is_run = false;
    std::cout << "Clothing server..." << std::endl;  
    m_sock.close();
    m_sock6.close();
}








