#include "eva/event_io.hpp"
#include "eva/log.hpp"
#include <boost/asio/completion_condition.hpp>

using namespace eva;
using boost::asio::ip::tcp;

namespace {
    eva::log& l = eva::logger::instance().get( "event_io" );
}

event_io_session::event_io_session( const event_io_callback_ptr& callback, int id, boost::asio::io_service& io ) :
    _callback( callback ),
    _id( id ),
    _socket( io )
{
    LOG_DEBUG( l, "new event_io_session @ " << this );
}

void event_io_session::write( const event& e )
{
    LOG_DEBUG( l, "event_io_session @ " << this << " write " << e.get_length() << " bytes" );
    boost::asio::async_write(
        _socket,
        boost::asio::buffer( &e.get_header(), e.get_length() ),
        boost::bind(
            &event_io_session::write_complete,
            shared_from_this(),
            boost::asio::placeholders::error ) );
}

void event_io_session::write_complete( const boost::system::error_code& error )
{
    LOG_DEBUG( l, "event_io_session @ " << this << " write complete" );
    if( error ) {
        stop();
    }
}

void event_io_session::read()
{
    LOG_DEBUG( l, "event_io_session @ " << this << " read" );
    event& e = _callback->alloc_event();
    e.get_header()._source = _id;
    boost::asio::async_read(
        _socket,
        boost::asio::buffer( (char*)&e.get_header(), sizeof( event_header ) ),
        boost::asio::transfer_exactly( sizeof( event_header ) ),
        boost::bind(
            &event_io_session::read_header_complete,
            shared_from_this(),
            &e,
            boost::asio::placeholders::bytes_transferred,
            boost::asio::placeholders::error ) );
}

void event_io_session::read_header_complete( event* e, size_t len, const boost::system::error_code& error )
{
    LOG_DEBUG( l, "event_io_session @ " << this << " read_header_complete " << len << " bytes" );
    if( !error && len ) {
        read_body( e );
    } else {
        stop();
    }
}

void event_io_session::read_body( event* e )
{
    LOG_DEBUG( l, "event_io_session @ " << this << " read_body" );
    boost::asio::async_read(
        _socket,
        boost::asio::buffer( e->get_body_buf(), e->get_header()._length ),
        boost::asio::transfer_exactly( e->get_header()._length ),
        boost::bind(
            &event_io_session::read_body_complete,
            shared_from_this(),
            e,
            boost::asio::placeholders::bytes_transferred,
            boost::asio::placeholders::error ) );
}

void event_io_session::read_body_complete( event* e, size_t len, const boost::system::error_code& error )
{
    LOG_DEBUG( l, "event_io_session @ " << this << " read_body_complete " << len << " bytes" );
    if( !error && len ) {
        _callback->on_event( *this, *e );
    } else {
        stop();
    }
}

void event_io_session::stop()
{
    LOG_DEBUG( l, "event_io_session @ " << this << " stop" );
    _callback->on_close( *this );
    if( _socket.is_open() ) {
        _socket.close();
    }
}

event_io_session_ptr event_io_controller::connect( const char* host, int port, const event_io_callback_ptr& callback )
{
    event_io_session_ptr sess = make_session( callback );
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string( host ), port );
    sess->_socket.connect( endpoint, ec );
    sess->_callback->on_open( *sess.get() );
    return sess;
}

void event_io_controller::accept( int port, const event_io_callback_ptr& callback )
{
    accept( new tcp::acceptor( _io, tcp::endpoint( tcp::v4(), port ) ), callback );
}

void event_io_controller::accept( tcp::acceptor* acceptor, const event_io_callback_ptr& callback )
{
    event_io_session_ptr sess = make_session( callback );
    acceptor->async_accept(
        sess->_socket,
        boost::bind(
            &event_io_controller::accept_complete,
            this,
            acceptor,
            sess,
            boost::asio::placeholders::error ) );
}

void event_io_controller::accept_complete( tcp::acceptor* acceptor, event_io_session_ptr session, const boost::system::error_code& error )
{
    if( !error ) {
        session->read();
        accept( acceptor, session->_callback );
    }
}

void event_io_controller::write( const event& e )
{
    event_io_session_ptr sess = _sessions[ e.get_header()._source ].lock();
    sess->write( e );
}

void event_io_controller::run()
{
    _io.run();
}

void event_io_controller::poll()
{
    _io.poll();
}

event_io_session_ptr event_io_controller::make_session( const event_io_callback_ptr& callback )
{
    event_io_session_ptr sess = event_io_session_ptr( new event_io_session( callback, _sessions.size(), _io ) );
    _sessions[ sess->_id ] = sess;
    return sess;
}
