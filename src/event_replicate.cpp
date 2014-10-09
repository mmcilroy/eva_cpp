#include "eva/event_replicate.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "event_replicate" );
}

struct replicate_callback : public event_io_callback
{
    event& alloc_event()
    {
        return *(event*)0;  // should never be called
    }
};

event_replicate_thread::event_replicate_thread( event_subscriber& s ) :
    event_node( "replicate" ),
    _subscriber( &s )
{
}

void event_replicate_thread::run()
{
    event_io_session_ptr sess = _ioc.connect( "0.0.0.0", 14004, event_io_callback_ptr( new replicate_callback ) );
    while( 1 )
    {
        const event& e = _subscriber->next();
        if( !e.get_header()._recovery )
        {
            LOG_INFO( l, "replicate: " << e.get_header() );
            sess->write( e );
            _ioc.poll();
        }
    }
}
