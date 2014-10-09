#pragma once

#include "event_queue.hpp"
#include <map>

namespace eva {

class event_application
{
public:
    static event_application& instance();

    event_queue* make_queue( const std::string&, event_queue_options opt=NONE );
    event_queue* find_queue( const std::string& );
    void recover_queue( const std::string& from, event_publisher& pub );

private:
    event_application();

    typedef std::map< std::string, event_queue* > queue_map;
    queue_map _queues;

    static event_application* _instance;
};

event_application& app();

}
