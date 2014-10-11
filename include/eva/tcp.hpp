#pragma once

#include "eva/event.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/unordered_map.hpp>

using boost::asio::ip::tcp;

namespace eva {

class tcp_session;

struct tcp_callback
{
    virtual void on_open( tcp_session& ) {}
    virtual void on_close( tcp_session& ) {}
    virtual void on_event( tcp_session&, const event& ) {}
    virtual event& alloc_event() = 0;
};

typedef boost::shared_ptr< tcp_callback > tcp_callback_ptr;

class tcp_session : public boost::enable_shared_from_this< tcp_session >
{
public:
    tcp_session( const tcp_callback_ptr& callback, int id, boost::asio::io_service& io );
    ~tcp_session();

    void write( const event& e );
    void write_complete( const boost::system::error_code& error );
    void read();
    void read_header_complete( event* e, size_t len, const boost::system::error_code& error );
    void read_body( event* e );
    void read_body_complete( event* e, size_t len, const boost::system::error_code& error );
    void stop();

//private:
    int _id;
    tcp_callback_ptr _callback;
    boost::asio::ip::tcp::socket _socket;
};

typedef boost::shared_ptr< tcp_session > tcp_session_ptr;

class tcp_controller
{
public:
    tcp_session_ptr connect(    const char* host, int port, const tcp_callback_ptr& callback );

    void accept( int port, const tcp_callback_ptr& callback );
    void accept( tcp::acceptor* acceptor, const tcp_callback_ptr& callback );
    void accept_complete( tcp::acceptor* acceptor, tcp_session_ptr session, const boost::system::error_code& error );
    void write( const event& e );
    void run();
    void poll();
    tcp_session_ptr make_session( const tcp_callback_ptr& callback );

//private:
    boost::asio::io_service _io;
    boost::unordered_map< int, boost::weak_ptr< tcp_session > > _sessions;
};

}
