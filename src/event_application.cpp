#include "eva/event_application.hpp"
#include "eva/eva.hpp"

using namespace eva;

event_application* event_application::_instance = 0;

event_application& event_application::instance()
{
    if( !_instance ) {
        _instance = new event_application;
    }
    return *_instance;
}

event_queue* event_application::make_queue( const std::string& name, event_queue_options opt )
{
    if( _queues.find( name ) != _queues.end() ) {
        eva_exception( "queue [" << name << "] already exists" );
    }
    return ( _queues[name] = new event_queue( opt ) );
}

event_queue* event_application::find_queue( const std::string& name )
{
    if( _queues.find( name ) == _queues.end() ) {
        eva_exception( "queue [" << name << "] does not exist" );
    }
    return _queues[name];
}

void event_application::recover_queue( const std::string& from, event_publisher& pub )
{
    find_queue( from )->recover( pub );
}

event_application::event_application()
{
}

namespace eva {
event_application& app()
{
    return event_application::instance();
}
}
