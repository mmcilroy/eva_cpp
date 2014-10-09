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

struct my_processor : public eva::event_node
{
    my_processor() :
        event_node( "my_processor" ),
        _inp0( subscribe( "source0" ) ),
        _inp1( subscribe( "source1" ) ),
        _inp2( subscribe( "source2" ) ),
        _out( publish( "sink0" ) )
    {
    }

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            const eva::event& e0 = _inp0->next();
            assert( e0.get_header()._id == i );

            const eva::event& e1 = _inp1->next();
            assert( e0.get_header()._id == i );

            const eva::event& e2 = _inp2->next();
            assert( e0.get_header()._id == i );

            eva::event& e = _out->next();
            e.get_header()._id = i;
            _out->commit();
        }
    }

    eva::event_subscriber* _inp0;
    eva::event_subscriber* _inp1;
    eva::event_subscriber* _inp2;
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
    eva::event_application::instance().make_queue( "source0" );
    eva::event_application::instance().make_queue( "source1" );
    eva::event_application::instance().make_queue( "source2" );
    eva::event_application::instance().make_queue( "sink0" );

    my_source source0( "source0" );
    my_source source1( "source1" );
    my_source source2( "source2" );
    my_processor processor;
    my_sink sink;

    eva::stopwatch watch;
    watch.start();

    sink.start();
    processor.start();
    source0.start();
    source1.start();
    source2.start();

    sink.join();
    processor.join();
    source0.join();
    source1.join();
    source2.join();

    std::cout << N << " events in " << watch.elapsed_ms() << " ms. " << (int)( N / watch.elapsed_s() ) << " per sec" << std::endl;
}
