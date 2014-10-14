#include "eva/tcp.hpp"
#include "eva/log.hpp"
#include <boost/asio/completion_condition.hpp>

using namespace eva;
using boost::asio::ip::tcp;

namespace {
    eva::log& l = eva::logger::instance().get( "tcp" );
}

tcp_session::tcp_session( const tcp_callback_ptr& callback, int id, boost::asio::io_service& io ) :
    _callback( callback ),
    _id( id ),
    _socket( io )
{
    LOG_DEBUG( l, "new tcp_session @ " << this << " id=" << _id );
}

tcp_session::~tcp_session()
{
    LOG_DEBUG( l, "del tcp_session @ " << this << " id=" << _id );
}

void tcp_session::write( const event& e )
{
    LOG_DEBUG( l, "tcp_session @ " << this << " write " << e.get_length() << " bytes" );
    boost::asio::async_write(
        _socket,
        boost::asio::buffer( &e.get_header(), e.get_length() ),
        boost::bind(
            &tcp_session::write_complete,
            shared_from_this(),
            boost::asio::placeholders::error ) );
}

void tcp_session::write_complete( const boost::system::error_code& error )
{
    LOG_DEBUG( l, "tcp_session @ " << this << " write complete" );
    if( error ) {
        stop();
    }
}

void tcp_session::read()
{
    LOG_DEBUG( l, "tcp_session @ " << this << " read, id=" << _id );
    event& e = _callback->alloc_event();
    boost::asio::async_read(
        _socket,
        boost::asio::buffer( (char*)&e.get_header(), sizeof( event_header ) ),
        boost::asio::transfer_exactly( sizeof( event_header ) ),
        boost::bind(
            &tcp_session::read_header_complete,
            shared_from_this(),
            &e,
            boost::asio::placeholders::bytes_transferred,
            boost::asio::placeholders::error ) );
}

void tcp_session::read_header_complete( event* e, size_t len, const boost::system::error_code& error )
{
    LOG_DEBUG( l, "tcp_session @ " << this << " read_header_complete " << len << " bytes" );
    if( !error && len ) {
        read_body( e );
    } else {
        stop();
    }
}

void tcp_session::read_body( event* e )
{
    LOG_DEBUG( l, "tcp_session @ " << this << " read_body" );
    boost::asio::async_read(
        _socket,
        boost::asio::buffer( e->get_body_buf(), e->get_header()._length ),
        boost::asio::transfer_exactly( e->get_header()._length ),
        boost::bind(
            &tcp_session::read_body_complete,
            shared_from_this(),
            e,
            boost::asio::placeholders::bytes_transferred,
            boost::asio::placeholders::error ) );
}

void tcp_session::read_body_complete( event* e, size_t len, const boost::system::error_code& error )
{
    LOG_DEBUG( l, "tcp_session @ " << this << " read_body_complete " << len << " bytes" );
    if( !error && len )
    {
        e->get_header()._source = _id;
        _callback->on_event( *this, *e );
    }
    else
    {
        stop();
    }
}

void tcp_session::stop()
{
    LOG_DEBUG( l, "tcp_session @ " << this << " stop" );
    _callback->on_close( *this );
    if( _socket.is_open() ) {
        _socket.close();
    }
}

tcp_session_ptr tcp_controller::connect( const char* host, int port, const tcp_callback_ptr& callback )
{
    LOG_DEBUG( l, "connect: host=" << host << ", port=" << port );
    tcp_session_ptr sess = make_session( callback );

    /*
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string( host ), port );
    */

    std::stringstream ss; ss << port;
    tcp::resolver resolver( _io );
    tcp::resolver::iterator it = resolver.resolve( tcp::resolver::query( host, ss.str() ) );
    boost::asio::connect( sess->_socket, it );
    sess->_callback->on_open( *sess.get() );
    return sess;
}

void tcp_controller::accept( int port, const tcp_callback_ptr& callback )
{
    std::stringstream ss; ss << port;
    boost::asio::ip::tcp::resolver resolver( _io );
    boost::asio::ip::tcp::resolver::query query( "0.0.0.0", ss.str() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
    accept( new tcp::acceptor( _io, endpoint ), callback );

    //accept( new tcp::acceptor( _io, tcp::endpoint( tcp::v4( "0.0.0.0" ), port ) ), callback );
}

void tcp_controller::accept( tcp::acceptor* acceptor, const tcp_callback_ptr& callback )
{
    tcp_session_ptr sess = make_session( callback );
    acceptor->async_accept(
        sess->_socket,
        boost::bind(
            &tcp_controller::accept_complete,
            this,
            acceptor,
            sess,
            boost::asio::placeholders::error ) );
}

void tcp_controller::accept_complete( tcp::acceptor* acceptor, tcp_session_ptr session, const boost::system::error_code& error )
{
    if( !error ) {
        session->read();
        accept( acceptor, session->_callback );
    }
}

void tcp_controller::write( const event& e )
{
    int source = e.get_header()._source;
    tcp_session_ptr sess = _sessions[source].lock();
    if( sess ) {
        sess->write( e );
    } else {
        LOG_DEBUG( l, "session " << source << " is no more" )
    }
}

void tcp_controller::run()
{
    _io.run();
}

void tcp_controller::poll()
{
    _io.poll();
}

tcp_session_ptr tcp_controller::make_session( const tcp_callback_ptr& callback )
{
    tcp_session_ptr sess = tcp_session_ptr( new tcp_session( callback, _sessions.size(), _io ) );
    _sessions[ sess->_id ] = sess;
    return sess;
}
