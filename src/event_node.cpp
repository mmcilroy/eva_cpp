#include "eva/event_application.hpp"
#include "eva/event_node.hpp"

using namespace eva;

event_node::event_node()
{
}

void event_node::start()
{
    _thread = boost::shared_ptr< boost::thread >(
        new boost::thread( boost::bind( &event_node::run, this ) ) );
}

void event_node::join()
{
    _thread->join();
}

event_subscriber* event_node::subscribe( const std::string& to )
{
    event_queue* queue = event_application::instance().find_queue( to );
    return queue->subscriber();
}

event_publisher* event_node::publish( const std::string& to )
{
    event_queue* queue = event_application::instance().find_queue( to );
    return queue->publisher();
}
