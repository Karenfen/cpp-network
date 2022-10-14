#include "transceiver.h"


bool Transceiver::send_buffer(const std::vector<char> &buffer)
{
    size_t transmit_bytes_count = 0;
    const auto size = buffer.size();

    while (transmit_bytes_count != size)
    {
        auto result = send(client_sock_, &(buffer.data()[0]) + transmit_bytes_count, size - transmit_bytes_count, 0);
        if (-1 == result)
        {
            if (need_to_repeat()) continue;
            return false;
        }

        transmit_bytes_count += result;
    }

    return true;
}

bool Transceiver::send_file(fs::path const& file_path)
{
    std::vector<char> buffer(buffer_size);
    std::ifstream file_stream(file_path, std::ifstream::binary);

    if (!file_stream) return false;

    std::cout << "Sending file " << file_path << "..." << std::endl;
    while (file_stream)
    {
        file_stream.read(&buffer[0], buffer.size());
        if (!send_buffer(buffer))
        {
            file_stream.close();
            return false;
        }
    }

    file_stream.close();
    return true;
}

bool Transceiver::send_file(fs::path const& file_path, const segment_info& segment)
{
    std::vector<char> buffer;

    if(segment.size == 0)
        buffer.resize(buffer_size);
    else
        buffer.resize(segment.size);

    std::ifstream file_stream(file_path, std::ifstream::binary);

    if (!file_stream) return false;

    std::cout << "Sending file segment" << file_path << "..." << std::endl;

    file_stream.seekg(segment.begin, std::ios::beg);
    if(file_stream.eof())
    {
        file_stream.close();
        return false;
    }
    file_stream.read(&buffer[0], buffer.size());
    if (!send_buffer(buffer))
    {
        file_stream.close();
        return false;
    }

    file_stream.close();
    return true;
}

std::pair<std::string, std::optional<segment_info>>  Transceiver::get_request()
{
    std::array<char, MAX_PATH + 1> buffer;
    size_t recv_bytes = 0;
    const auto size = buffer.size() - 1;

    std::cout << "Reading user request..." << std::endl;
    while (true)
    {
        auto result = recv(client_sock_, &buffer[recv_bytes], size - recv_bytes, 0);

        if (!result) break;

        if (-1 == result)
        {
            if (need_to_repeat()) continue;
            throw std::logic_error("Socket reading error");
        }

        auto fragment_begin = buffer.begin() + recv_bytes;
        auto ret_iter = std::find_if(fragment_begin, fragment_begin + result,
                                     [](char sym) { return '\n' == sym || '\r' == sym;  });
        if (ret_iter != buffer.end())
        {
            *ret_iter = '\0';
            recv_bytes += std::distance(fragment_begin, ret_iter);
            break;
        }
        recv_bytes += result;
        if (size == recv_bytes) break;
    }

    buffer[recv_bytes] = '\0';

    auto str_result = std::string(buffer.begin(), buffer.begin() + recv_bytes);
    std::cout << "Request = \"" << str_result << "\"" << std::endl;

    auto result = extract_segment_info_from_request(str_result);

    return result;
}


bool Transceiver::need_to_repeat()
{
    switch (errno)
    {
        case EINTR:
        case EAGAIN:
        // case EWOULDBLOCK: // EWOULDBLOCK == EINTR.
            return true;
    }

    return false;
}

std::pair<std::string, std::optional<segment_info>> Transceiver::extract_segment_info_from_request(const std::string &request)
{
    std::string path;
    segment_info info = {
        .begin = 0,
        .size = 0
    };

    std::stringstream stream_request;
    stream_request << request;
    stream_request >> path >> info.begin >> info.size;

    if(info.begin == 0 && info.size == 0)
        return std::make_pair(request, std::nullopt);

    return std::make_pair(path, info);
};
