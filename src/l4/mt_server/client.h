#ifndef CLIENT_H
#define CLIENT_H


#include <optional>


#include "transceiver.h"



class Client
{
public:
    Client(socket_wrapper::Socket &&sock);

    std::pair<std::optional<fs::path>, std::optional<segment_info>> recv_file_path();

    bool send_file(const fs::path &file_path, std::optional<segment_info> info = std::nullopt);

    bool process();

private:
    Transceiver tsr_;
    fs::path file_path_;
};

#endif // CLIENT_H
