#include "eva/event_replicate.hpp"

using namespace eva;

event_replicate_thread::event_replicate_thread( event_subscriber& s ) :
    _subscriber( &s )
{
}

void event_replicate_thread::run()
{
    while( 1 ) {
        const event& e = _subscriber->next();
    }
}
