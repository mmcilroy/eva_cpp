#include "eva/event_application.hpp"
#include "eva/event_node.hpp"
#include "eva/stopwatch.hpp"

const int N = 10*1024*1024;

struct my_source : public eva::event_node
{
    my_source( const std::string& name ) :
        event_node( "my_source" ),
        _out( publish( name ) )
    {
    }

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            eva::event& e = _out->next();
            e.get_header()._id = i;
            _out->commit();
        }
    }

    eva::event_publisher* _out;
};

struct my_sink : public eva::event_node
{
    my_sink() :
        event_node( "my_sink" ),
        _inp( subscribe( "sink0" ) )
    {
    }

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            const eva::event& e = _inp->next();
            assert( e.get_header()._id == i );
        }
    }

    eva::event_subscriber* _inp;
};

int main()
{
    eva::event_application::instance().make_queue( "sink0" );

    my_source source0( "sink0" );
    my_sink sink;

    eva::stopwatch watch;
    watch.start();

    sink.start();
    source0.start();

    sink.join();
    source0.join();

    std::cout << N << " events in " << watch.elapsed_ms() << " ms. " << (int)( N / watch.elapsed_s() ) << " per sec" << std::endl;
}
