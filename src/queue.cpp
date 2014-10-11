#include "eva/queue.hpp"
#include "eva/journal.hpp"
#include "eva/replicate.hpp"
#include "eva/eva.hpp"

using namespace eva;

queue::queue( queue_options opt ) :
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
        _subscriber = new eva::subscriber( _events );
    }
    else if( opt == JOURNAL )
    {
        _journal_subscriber = new eva::subscriber( _events );
        _subscriber = new eva::subscriber( *_journal_subscriber );
    }
    else if( opt == REPLICATE )
    {
        _journal_subscriber = new eva::subscriber( _events );
        _replicate_subscriber = new eva::subscriber( *_journal_subscriber );
        _subscriber = new eva::subscriber( *_replicate_subscriber );
    }

    _publisher = new eva::publisher( *_subscriber );

    if( _journal_subscriber )
    {
        _journal_thread = new eva::journal_thread( *_journal_subscriber );
        _journal_thread->start();
    }

    if( _replicate_subscriber )
    {
        _replicate_thread = new eva::replicate_thread( *_replicate_subscriber );
        _replicate_thread->start();
    }
}

subscriber* queue::subscriber()
{
    return _subscriber;
}

publisher* queue::publisher()
{
    return _publisher;
}

void queue::recover()
{
    if( _journal_thread ) {
        _journal_thread->recover( *_publisher );
    }
}
