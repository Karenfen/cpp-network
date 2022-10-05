#include "logger.h"
#include <iostream>


logger::logger()
{

}

logger::~logger()
{
    if(file.is_open())
        file.close();
}


bool logger::write(const std::string &data)
{
    file.open(FileName, std::ofstream::out | std::ofstream::app);

    if(file.is_open())
    {
        file << data;
        file.close();
        return true;
    }
    else
    {
        std::cerr << "File write error!" << std::endl;
        return false;
    }
}
