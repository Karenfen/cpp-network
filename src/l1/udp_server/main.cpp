#include "udp_server.h"




int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port to ip4> " <<std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        boost::asio::io_context context;

        udp_server server(std::stoi(argv[1]), context);

        server.init();

//        if(!server.start())
//        {
//            throw std::logic_error(server.get_last_error());
//        }

        context.run();

    }  catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }


    return EXIT_SUCCESS;
}

