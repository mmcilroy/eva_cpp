#pragma once

#include "event_queue.hpp"
#include <map>

namespace eva {

class event_application
{
public:
    static event_application& instance();

    event_queue* make_queue( const std::string& );
    event_queue* find_queue( const std::string& );

private:
    event_application();

    typedef std::map< std::string, event_queue* > queue_map;
    queue_map _queues;

    static event_application* _instance;
};

}
