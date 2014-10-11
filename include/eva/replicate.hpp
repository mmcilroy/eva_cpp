#pragma once

#include "eva/thread.hpp"
#include "eva/queue.hpp"
#include "eva/tcp.hpp"

namespace eva {

class replicate_thread : public thread
{
public:
    replicate_thread( subscriber& );

protected:
    virtual void run();

private:
    subscriber* _subscriber;
    tcp_controller _tcp;
};

}
