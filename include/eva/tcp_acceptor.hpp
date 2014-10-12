#pragma once

#include "eva/thread.hpp"
#include "eva/queue.hpp"
#include "eva/tcp.hpp"

namespace eva {

class tcp_acceptor : public thread
{
public:
    tcp_acceptor( int port, queue& queue );

    void write( const event& e );

protected:
    virtual void run();

private:
    tcp_controller _tcp;
};

}
