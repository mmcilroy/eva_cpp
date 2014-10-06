#include "eva/event_queue.hpp"
#include <boost/thread.hpp>
#include <assert.h>
#include <iostream>

eva::event_queue queue;
eva::event_publisher* pub = queue.publisher();
eva::event_subscriber* sub = queue.subscriber();

const int N = 1024*1024;

void publish( int id )
{
    eva::event& e = pub->next();
    e.get_header()._id = id;
    pub->commit();
}

void subscribe( int id )
{
    const eva::event& e = sub->next();
    assert( e.get_header()._id == id );
}

struct thread
{
    thread()
    {
        _thread = boost::shared_ptr< boost::thread >( new boost::thread( boost::bind( &thread::run, this ) ) );
    }

    void join()
    {
        _thread->join();
    }

    virtual void run() = 0;

    boost::shared_ptr< boost::thread > _thread;
};

struct publisher : public thread
{
    void run()
    {
        for( int i=0; i<N; i++ ) {
            publish( i );
        }
    }
};

struct subscriber : public thread
{
    void run()
    {
        for( int i=0; i<N; i++ ) {
            subscribe( i );
        }
    }
};

int main()
{
    publisher p;
    subscriber s;
    p.join();
    s.join();
}
