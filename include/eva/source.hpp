#pragma once

#include "eva/thread.hpp"
#include "eva/queue.hpp"

namespace eva {

class source : public thread
{
public:
    source( queue& );

    event& next();
    void commit();

protected:
    virtual void run();
    virtual void loop();

private:
    queue* _queue;
    publisher* _out;
};

}
