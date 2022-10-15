#include "udp_server.h"




int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" <<std::endl;
        return EXIT_FAILURE;
    }

    udp_server server(std::stoi(argv[1]));
    server.init();

    if(!server.start())
    {
        std::cerr << server.get_last_error() << std::endl;
        return EXIT_FAILURE;
    }

    server.run();

    return EXIT_SUCCESS;
}

