#include "eva/event_application.hpp"
#include "eva/event_node.hpp"

const int N = 32;

boost::mutex _mutex;

#define log( m ) { std::stringstream ss; ss << __FILE__ << ":" << __LINE__ << " - " << boost::this_thread::get_id() << " " << m; __log( ss.str() ); }

void __log( const std::string& s )
{
    boost::lock_guard< boost::mutex > lock( _mutex );
    std::cout << s << std::endl;
}

struct my_source : public eva::event_node
{
    my_source( const std::string& name ) :
        _out( publish( name ) )
    {
    }

    virtual void start()
    {
        log( "starting source" );
        for( int i=0; i<N; i++ )
        {
            eva::event& e = _out->next();
            e.get_header()._id = i;
            _out->commit();
            log( "source published " << i );
        }
        log( "completed source" );
    }

    eva::event_publisher* _out;
};

struct my_processor : public eva::event_node
{
    my_processor() :
        _inp( subscribe( "source0" ) ),
        _out( publish( "sink0" ) )
    {
    }

    virtual void start()
    {
        log( "starting processor" );
        for( int i=0; i<N; i++ )
        {
            const eva::event& e0 = _inp->next();
            log( "processor got " << e0.get_header()._id << " excepted " << i );
            assert( e0.get_header()._id == i );

            eva::event& e1 = _out->next();
            e1.get_header()._id = i;
            _out->commit();
            log( "processor published " << i );
        }
        log( "completed processor" );
    }

    eva::event_subscriber* _inp;
    eva::event_publisher* _out;
};

struct my_sink : public eva::event_node
{
    my_sink() :
        _inp( subscribe( "sink0" ) )
    {
    }

    virtual void start()
    {
        log( "starting sink" );
        for( int i=0; i<N; i++ )
        {
            const eva::event& e = _inp->next();
            log( "sink got " << e.get_header()._id << " excepted " << i );
            assert( e.get_header()._id == i );
        }
        log( "completed sink" );
    }

    eva::event_subscriber* _inp;
};

int main()
{
    eva::event_application::instance().make_queue( "source0" );
    eva::event_application::instance().make_queue( "source1" );
    eva::event_application::instance().make_queue( "source2" );
    eva::event_application::instance().make_queue( "sink0" );

    my_source source( "sink0" );
    my_processor processor;
    my_sink sink;

    sink.start();
    //processor.start();
    source.start();

    source.join();
    //processor.join();
    sink.join();
}
