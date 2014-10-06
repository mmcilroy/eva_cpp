#include "eva/event_queue.hpp"
#include <assert.h>
#include <iostream>

eva::event_queue queue;
eva::event_publisher* pub = queue.publisher();
eva::event_subscriber* sub = queue.subscriber();

void publish( int id )
{
    std::cout << "publish " << id << std::endl;
    eva::event& e = pub->next();
    e.get_header()._id = id;
    pub->commit();
}

void subscribe( int id )
{
    std::cout << "subscribe " << id << std::endl;
    const eva::event& e = sub->next();
    assert( e.get_header()._id == id );
}

int main()
{
    for( int i=0; i<10; i++ ) {
        publish( i );
    }

    for( int i=0; i<10; i++ ) {
        subscribe( i );
    }
}
