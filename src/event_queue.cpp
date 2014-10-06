#include "eva/event_queue.hpp"

using namespace eva;

event_queue::event_queue( event_queue_options opt ) :
    _events( 1024 ),
    _subscriber( 0 ),
    _publisher( 0 )
{
    _subscriber = new event_subscriber( _events );
    _publisher = new event_publisher( *_subscriber );
}

event_subscriber* event_queue::subscriber()
{
    return _subscriber;
}

event_publisher* event_queue::publisher()
{
    return _publisher;
}
