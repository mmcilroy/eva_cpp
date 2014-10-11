#include "eva/replicate.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "replicate" );
}

struct replicate_callback : public tcp_callback
{
    event& alloc_event()
    {
        return *(event*)0;  // should never be called
    }
};

replicate_thread::replicate_thread( subscriber& s ) :
    _subscriber( &s )
{
}

void replicate_thread::run()
{
    tcp_session_ptr sess = _tcp.connect( "0.0.0.0", 14004, tcp_callback_ptr( new replicate_callback ) );
    while( _run )
    {
        const event& e = _subscriber->next();
        if( !e.get_header()._recovery )
        {
            LOG_INFO( l, "replicate: " << e.get_header() );
            sess->write( e );
            _tcp.poll();
        }
    }
}
