#pragma once

#include "eva/disruptor.hpp"
#include "eva/event.hpp"

namespace eva {

class event_journal_thread;
class event_replicate_thread;

typedef ring_buffer< event > event_ring_buffer;
typedef reader< event > event_subscriber;
typedef writer< event > event_publisher;

enum event_queue_options
{
    NONE = 0,
    JOURNAL = 1,
    REPLICATE = 2
};

class event_queue
{
public:
    event_queue( event_queue_options opt=NONE );
    event_subscriber* subscriber();
    event_publisher* publisher();

private:
    event_ring_buffer _events;
    event_publisher* _publisher;
    event_subscriber* _subscriber;
    event_subscriber* _journal_subscriber;
    event_subscriber* _replicate_subscriber;
    event_journal_thread* _journal_thread;
    event_replicate_thread* _replicate_thread;

};

}

// notes
// ---------------------------------------------------------------------------
// use unique_ptr instead of raw pointers?
