#ifndef SERVER_H
#define SERVER_H


#include <algorithm>
#include <future>
#include <list>
#include <stdexcept>
#include <string>
#include <thread>
#include <cerrno>
#include <cassert>




#include "client.h"


#if !defined(_WIN32)
extern "C"
{
#   include <signal.h>
}
#else
#   include <cwctype>
#endif




const auto clients_count = 10;





#endif // SERVER_H
