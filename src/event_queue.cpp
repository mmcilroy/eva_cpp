#include "eva/event_queue.hpp"
#include "eva/event_journal.hpp"
#include "eva/event_replicate.hpp"
#include "eva/eva.hpp"

using namespace eva;

event_queue::event_queue( event_queue_options opt ) :
    _events( 8*1024 ),
    _publisher( 0 ),
    _subscriber( 0 ),
    _journal_subscriber( 0 ),
    _replicate_subscriber( 0 ),
    _journal_thread( 0 ),
    _replicate_thread( 0 )
{
    if( opt == NONE )
    {
        _subscriber = new event_subscriber( _events );
    }
    else if( opt == JOURNAL )
    {
        _journal_subscriber = new event_subscriber( _events );
        _subscriber = new event_subscriber( *_journal_subscriber );
    }
    else if( opt == REPLICATE )
    {
        _journal_subscriber = new event_subscriber( _events );
        _replicate_subscriber = new event_subscriber( *_journal_subscriber );
        _subscriber = new event_subscriber( *_replicate_subscriber );
    }

    _publisher = new event_publisher( *_subscriber );

    if( _journal_subscriber )
    {
        _journal_thread = new event_journal_thread( *_journal_subscriber );
        _journal_thread->start();
    }

    if( _replicate_subscriber )
    {
        _replicate_thread = new event_replicate_thread( *_replicate_subscriber );
        _replicate_thread->start();
    }
}

event_subscriber* event_queue::subscriber()
{
    return _subscriber;
}

event_publisher* event_queue::publisher()
{
    return _publisher;
}

void event_queue::recover( event_publisher& pub )
{
    if( _journal_thread ) {
        _journal_thread->recover( pub );
    }
}
