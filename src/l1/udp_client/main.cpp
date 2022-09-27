//#include <algorithm>
//#include <cstdlib>
//#include <iomanip>
#include <iostream>
//#include <string>

#include "udp_client.h"


int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip address> " << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    udp_client client(argv[1], std::stoi(argv[2]));

    if(!client.run_session())
    {
        std::cerr << client.get_last_error() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

