#include "client.h"




Client::Client(socket_wrapper::Socket &&sock) :
    tsr_(std::move(sock))
{
    std::cout
        << "Client ["<< static_cast<int>(tsr_.ts_socket()) << "] "
        << "was created..."
        << std::endl;
}

std::pair<std::optional<fs::path>, std::optional<segment_info>> Client::recv_file_path()
{
    auto request_data = tsr_.get_request();
    if (!request_data.first.size()) return std::make_pair(std::nullopt, std::nullopt);

    auto cur_path = fs::current_path().wstring();
    auto file_path = fs::weakly_canonical(request_data.first).wstring();

#if defined(_WIN32)
    std::transform(cur_path.begin(), cur_path.end(), cur_path.begin(),
        [](wchar_t c) { return std::towlower(c); }
    );
    std::transform(file_path.begin(), file_path.end(), file_path.begin(),
        [](wchar_t c) { return std::towlower(c); }
    );
#endif
    if (file_path.find(cur_path) == 0)
    {
        file_path = file_path.substr(cur_path.length());
    }

    return std::make_pair(fs::weakly_canonical(cur_path + separ + file_path), request_data.second);
}

bool Client::send_file(const fs::path &file_path, std::optional<segment_info> info)
{
    if (!(fs::exists(file_path) && fs::is_regular_file(file_path))) return false;

    if(info == std::nullopt)
        return tsr_.send_file(file_path);

    return tsr_.send_file(file_path, info.value());
}

bool Client::process()
{
    auto file_to_send = recv_file_path();
    bool result = false;

    if (std::nullopt != file_to_send.first)
    {
        std::cout << "Trying to send " << file_to_send.first.value() << "..." << std::endl;
        if (send_file(file_to_send.first.value(), file_to_send.second))
        {
            std::cout << "File was sent." << std::endl;
        }
        else
        {
            std::cerr << "File sending error!" << std::endl;
        }
        result = true;
    }

    return result;
}
