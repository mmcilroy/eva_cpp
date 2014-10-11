#pragma once

#include "eva/disruptor.hpp"
#include "eva/event.hpp"

namespace eva {

class journal_thread;
class replicate_thread;

typedef ring_buffer< event > event_ring_buffer;
typedef reader< event > subscriber;
typedef writer< event > publisher;

enum queue_options
{
    NONE = 0,
    JOURNAL = 1,
    REPLICATE = 2
};

class queue
{
public:
    queue( eva::queue_options opt=NONE );

    eva::subscriber* subscriber();
    eva::publisher* publisher();
    void recover();

private:
    eva::event_ring_buffer _events;
    eva::publisher* _publisher;
    eva::subscriber* _subscriber;
    eva::subscriber* _journal_subscriber;
    eva::subscriber* _replicate_subscriber;
    eva::journal_thread* _journal_thread;
    eva::replicate_thread* _replicate_thread;};
}
