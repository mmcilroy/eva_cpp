#pragma once

struct event_payload
{
    char _data[16];
};

std::ostream& operator<<( std::ostream& o, const eva::event& e )
{
    o << e.get_header() << " | event_payload: data=" << e.get_body<event_payload>()._data;
    return o;
}

void write_event_payload( eva::event& e, int id )
{
    std::stringstream ss;
    ss << id;

    eva::event_header& eh = e.get_header();
    event_payload& ep = e.get_body<event_payload>();
    eh._id = id;
    eh._length = sizeof( event_payload );
    strcpy( ep._data, ss.str().c_str() );
}
