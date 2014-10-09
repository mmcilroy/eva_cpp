#include "eva/event_application.hpp"
#include "eva/event_node.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "event_node" );
}

event_node::event_node( const std::string& name )
: _name( name )
{
}

void event_node::start()
{
    _thread = boost::shared_ptr< boost::thread >(
        new boost::thread( boost::bind( &event_node::run_node, this ) ) );
}

void event_node::join()
{
    _thread->join();
}

void event_node::run_node()
{
    LOG_INFO( l, "start node " << _name );
    run();
    LOG_INFO( l, "stop node " << _name );    
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

void event_node::recover( const std::string& from, event_publisher& pub )
{
    event_application::instance().recover_queue( from, pub );
}
