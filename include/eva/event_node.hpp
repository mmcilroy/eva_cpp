#pragma once

#include "event_queue.hpp"
#include <boost/thread.hpp>
#include <string>

namespace eva {

class event_node
{
public:
    event_node();
    void join();
    virtual void start() = 0;

protected:
    event_subscriber* subscribe( const std::string& );
    event_publisher* publish( const std::string& );

private:
    boost::shared_ptr< boost::thread > _thread;
};

}
