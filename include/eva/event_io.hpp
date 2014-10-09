#pragma once

#include "eva/event.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/unordered_map.hpp>

using boost::asio::ip::tcp;

namespace eva {

class event_io_session;

struct event_io_callback
{
    virtual void on_open( event_io_session& ) {}
    virtual void on_close( event_io_session& ) {}
    virtual void on_event( event_io_session&, const event& ) {}
    virtual event& alloc_event() = 0;
};

typedef boost::shared_ptr< event_io_callback > event_io_callback_ptr;

class event_io_session : public boost::enable_shared_from_this< event_io_session >
{
public:
    event_io_session( const event_io_callback_ptr& callback, int id, boost::asio::io_service& io );
    ~event_io_session();

    void write( const event& e );
    void write_complete( const boost::system::error_code& error );
    void read();
    void read_header_complete( event* e, size_t len, const boost::system::error_code& error );
    void read_body( event* e );
    void read_body_complete( event* e, size_t len, const boost::system::error_code& error );
    void stop();

//private:
    int _id;
    event_io_callback_ptr _callback;
    boost::asio::ip::tcp::socket _socket;
};

typedef boost::shared_ptr< event_io_session > event_io_session_ptr;

class event_io_controller
{
public:
    event_io_session_ptr connect( const char* host, int port, const event_io_callback_ptr& callback );

    void accept( int port, const event_io_callback_ptr& callback );
    void accept( tcp::acceptor* acceptor, const event_io_callback_ptr& callback );
    void accept_complete( tcp::acceptor* acceptor, event_io_session_ptr session, const boost::system::error_code& error );
    void write( const event& e );
    void run();
    void poll();
    event_io_session_ptr make_session( const event_io_callback_ptr& callback );

//private:
    boost::asio::io_service _io;
    boost::unordered_map< int, boost::weak_ptr< event_io_session > > _sessions;
};

}
