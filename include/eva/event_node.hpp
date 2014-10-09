#pragma once

#include "event_queue.hpp"
#include <boost/thread.hpp>
#include <string>

namespace eva {

class event_node
{
public:
    event_node( const std::string& name );
    void start();
    void join();

protected:
    virtual void run() = 0;
    event_subscriber* subscribe( const std::string& );
    event_publisher* publish( const std::string& );
    void recover( const std::string&, event_publisher& );

private:
    void run_node();

    std::string _name;
    boost::shared_ptr< boost::thread > _thread;
};

}
