#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>


const std::string FileName{"log.txt"};


class logger
{
public:
    logger();
    ~logger();
    bool write(const std::string& data);

private:
    std::ofstream file;
};

#endif // LOGGER_H
