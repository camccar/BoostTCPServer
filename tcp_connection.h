#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

/**
 * @todo write docs
 */

using boost::asio::ip::tcp;
class tcp_connection 
: public boost::enable_shared_from_this<tcp_connection>
{

    public:
        typedef boost::shared_ptr<tcp_connection> pointer;
        void start();
        
            static pointer create(boost::asio::io_context& io_context)
            {
                return pointer(new tcp_connection(io_context));
            }
        
            tcp::socket& socket();
        
    private:
        tcp_connection(boost::asio::io_context& io_context)
        : socket_(io_context)
        {
        }
        
        void handle_write(const boost::system::error_code& /*error*/,
        size_t /*bytes_transferred*/)
        {
        }
    
        boost::asio::ip::tcp::socket socket_;
        std::string message_;
};

#endif // TCP_CONNECTION_H
