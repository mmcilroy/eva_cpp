#pragma once

#include "eva/thread.hpp"
#include "eva/queue.hpp"

namespace eva {

class source : public thread
{
public:
    source( queue& );

    eva::publisher& publisher();
    eva::event& next();
    void commit();

protected:
    virtual void run();
    virtual void loop();

private:
    eva::queue* _queue;
    eva::publisher* _out;
};

}
