#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H


#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>




#if !defined(MAX_PATH)
#   define MAX_PATH (256)
#endif


struct segment_info{
    size_t begin;
    size_t size;
};


const auto buffer_size = 4096;


namespace fs = std::filesystem;


#if defined(_WIN32)
const wchar_t separ = fs::path::preferred_separator;
#else
const wchar_t separ = *reinterpret_cast<const wchar_t*>(&fs::path::preferred_separator);
#endif



class Transceiver
{
public:
    Transceiver(socket_wrapper::Socket &&client_sock) : client_sock_(std::move(client_sock)) {}
    Transceiver(const Transceiver&) = delete;
    Transceiver() = delete;

public:
    const socket_wrapper::Socket &ts_socket() const { return client_sock_; }

public:
    bool send_buffer(const std::vector<char> &buffer);
    bool send_file(fs::path const& file_path);
    bool send_file(fs::path const& file_path, const segment_info& segment);
    std::pair<std::string, std::optional<segment_info>>  get_request();

private:
    static bool need_to_repeat();
    std::pair<std::string, std::optional<segment_info>> extract_segment_info_from_request(const std::string& request);

private:
    socket_wrapper::Socket client_sock_;
};

#endif // TRANSCEIVER_H
