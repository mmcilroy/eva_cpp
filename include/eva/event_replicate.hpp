#pragma once

#include "eva/event_node.hpp"
#include "eva/event_queue.hpp"
#include "eva/event_io.hpp"

namespace eva {

class event_replicate_thread : public event_node
{
public:
    event_replicate_thread( event_subscriber& );

protected:
    virtual void run();

private:
    event_subscriber* _subscriber;
    event_io_controller _ioc;
};

}
