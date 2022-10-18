
#include  "server.h"

using namespace std::literals;


std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>
get_serv_info(const char *port)
{
    struct addrinfo hints =
    {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    };
    struct addrinfo *s_i;
    int ai_status;

    if ((ai_status = getaddrinfo(nullptr, port, &hints, &s_i)) != 0)
    {
        std::cerr << "getaddrinfo error " << gai_strerror(ai_status) << std::endl;
        return std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(nullptr, freeaddrinfo);
    }

    return std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(s_i, freeaddrinfo);
}


void set_reuse_addr(socket_wrapper::Socket &sock)
{
    const int flag = 1;

    // Allow reuse of port.
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&flag), sizeof(flag)) < 0)
    {
        throw std::logic_error("Set SO_REUSEADDR error");
    }
}


socket_wrapper::Socket accept_client(socket_wrapper::Socket &server_sock)
{
    struct sockaddr_storage client_addr;
    socklen_t client_addr_length = sizeof(client_addr);
    std::array<char, INET_ADDRSTRLEN> addr;

    socket_wrapper::Socket client_sock(accept(server_sock, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_length));

    if (!client_sock)
    {
        throw std::logic_error("Accepting client");
    }

    assert(sizeof(sockaddr_in) == client_addr_length);

    std::cout <<
        "Client from " << inet_ntop(AF_INET, &(reinterpret_cast<const sockaddr_in * const>(&client_addr)->sin_addr), &addr[0], addr.size())
        << "..."
        << std::endl;
    return client_sock;
}


int main(int argc, const char * const argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

#if !defined(_WIN32)
    signal(SIGPIPE, SIG_IGN);
#endif

    socket_wrapper::SocketWrapper sock_wrap;

    try
    {
        auto servinfo = get_serv_info(argv[1]);
        if (!servinfo)
        {
            std::cerr << "Can't get servinfo!" << std::endl;
            exit(EXIT_FAILURE);
        }

        socket_wrapper::Socket server_sock = {servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol};

        if (!server_sock)
        {
            throw std::logic_error("Socket creation error");
        }

        set_reuse_addr(server_sock);

        if (bind(server_sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        {
            throw std::logic_error("Bind error");
        }

        std::list<std::future<bool>> pending_tasks;

        std::cout
            << "Listening on port " << argv[1] << "...\n"
            << "Server path: " << std::filesystem::current_path()
            << std::endl;

        if (listen(server_sock, clients_count) < 0)
        {
            throw std::logic_error("Listen error");
        }

        std::cout << "Listen was run..." << std::endl;

        while (true)
        {
            auto client_sock = accept_client(server_sock);

            if (!client_sock)
            {
                throw std::logic_error("Client socket error");
            }

            pending_tasks.push_back(std::async(std::launch::async, [&](socket_wrapper::Socket &&sock)
            {
                Client client(std::move(sock));
                std::cout << "Client tid = " << std::this_thread::get_id() << std::endl;
                auto result = client.process();
                std::cout
                    << "Client with tid = " << std::this_thread::get_id()
                    << " exiting..."
                    << std::endl;

                return result;
            }, std::move(client_sock)));

            std::cout << "Cleaning tasks..." << std::endl;
            for (auto task = pending_tasks.begin(); task != pending_tasks.end();)
            {
                if (std::future_status::ready == task->wait_for(1ms))
                {
                    auto fu = task++;
                    std::cout
                        << "Request completed with a result = " << fu->get() << "...\n"
                        << "Removing from list." << std::endl;
                    pending_tasks.erase(fu);
                }
                else ++task;
            }
        }
    }
    catch (const std::logic_error &e)
    {
        std::cerr
            << e.what()
            << " [" << sock_wrap.get_last_error_string() << "]!"
            << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

