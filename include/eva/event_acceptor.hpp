#pragma once

#include "eva/event_io.hpp"
#include "eva/event_node.hpp"

namespace eva {

class event_acceptor : public event_node
{
public:
    event_acceptor( int port, const std::string& name );
    void write( const event& e );

protected:
    virtual void run();

private:
    event_io_controller _ioc;
};

}
