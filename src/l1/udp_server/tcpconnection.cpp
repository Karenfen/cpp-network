#include "tcpconnection.h"

#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>



std::shared_ptr<TCPconnection> TCPconnection::create(boost::asio::io_context &context)
{
    return smart_pointer(new TCPconnection(context));
}

void TCPconnection::start()
{
    try
    {
        m_recive_data();
    }
    catch (std::exception& e)
    {
        std::cerr << "Session is aborted!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

TCPconnection::TCPconnection(boost::asio::io_context &context): m_socket(context)
{

}


void TCPconnection::m_recive_data()
{
    auto this_ptr = shared_from_this();

    //    boost::asio::async_read(m_socket, boost::asio::buffer(m_buffer), [this_ptr](const boost::system::error_code& error, size_t bytes_recived)
//    {
//        this_ptr->m_handle_read(error, bytes_recived);
//    });
        m_socket.async_read_some(boost::asio::buffer(m_buffer_read),
                            [this_ptr](const boost::system::error_code& error, size_t bytes_recived)
                             {
                                 this_ptr->m_handle_read(error, bytes_recived);
                             });
}

void TCPconnection::m_handle_read(const boost::system::error_code &, size_t bytes_recived)
{
    std::cout << "Bytes recived: " << bytes_recived
    << " from address: " << m_socket.remote_endpoint().address() << std::endl;

    // выполняем обработку запроса
    m_some_working();

    // отправляем ответ
    m_send_data(std::string(m_buffer_write, bytes_recived));
}


void TCPconnection::m_send_data(const std::string &data)
{
    auto this_ptr = shared_from_this();

    boost::asio::async_write(m_socket, boost::asio::buffer(data), [this_ptr](const boost::system::error_code& error, size_t bytes_transferred)
    {
        this_ptr->m_handle_write(error, bytes_transferred);
    });
}

void TCPconnection::m_some_working()
{
    for(int iter{0}; iter < SIZE_BUFFER; ++iter)
    {
        m_buffer_write[iter] = m_buffer_read[iter];
    }
}



void TCPconnection::m_handle_write(const boost::system::error_code&, size_t bytes_transferred)
{
    std::cout << "Bytes transferred: " << bytes_transferred
    << " to address: " << m_socket.remote_endpoint().address() << std::endl;

    start();
}


