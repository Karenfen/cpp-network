#include "udp_server.h"




int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <port to ip4> " << " <port to ip6>" <<std::endl;
        return EXIT_FAILURE;
    }

    udp_server server(std::stoi(argv[1]), std::stoi(argv[2]));
    server.init();

    if(!server.start())
    {
        std::cerr << server.get_last_error() << std::endl;
        return EXIT_FAILURE;
    }

    server.run();

    return EXIT_SUCCESS;
}

