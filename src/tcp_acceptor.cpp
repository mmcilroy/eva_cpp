#include "eva/tcp_acceptor.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "acceptor" );
}

struct acceptor_callback : public eva::tcp_callback
{
    acceptor_callback( publisher* out ) :
        _out( out )
    {
    }

    void on_event( tcp_session& sess, const event& e )
    {
        LOG_DEBUG( l, "acceptor: " << e.get_header() );
        _out->commit();
        sess.read();
    }

    eva::event& alloc_event()
    {
        return _out->next();
    }

    eva::publisher* _out;
};

tcp_acceptor::tcp_acceptor( int port, queue* queue )
{
    _tcp.accept( port, tcp_callback_ptr(
        new acceptor_callback( queue->publisher() ) ) );
}

void tcp_acceptor::write( const event& e )
{
    _tcp.write( e );
}

void tcp_acceptor::run()
{
    _tcp.run();
}
