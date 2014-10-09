#include "eva/event_acceptor.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "event_acceptor" );
}

struct acceptor_callback : public eva::event_io_callback
{
    acceptor_callback( event_publisher* out ) :
        _out( out )
    {
    }

    void on_event( event_io_session& sess, const event& e )
    {
        LOG_DEBUG( l, "acceptor: " << e.get_header() );
        _out->commit();
        sess.read();
    }

    eva::event& alloc_event()
    {
        return _out->next();
    }

    eva::event_publisher* _out;
};

event_acceptor::event_acceptor( int port, const std::string& name ) :
    event_node( name )
{
    _ioc.accept( port, event_io_callback_ptr(
        new acceptor_callback( publish( name ) ) ) );
}

void event_acceptor::write( const event& e )
{
    _ioc.write( e );
}

void event_acceptor::run()
{
    _ioc.run();
}
