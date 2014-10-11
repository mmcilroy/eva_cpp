#include "eva/source.hpp"
#include "eva/stopwatch.hpp"

const int N = 1024*1024;

struct test_source : public eva::source
{
    test_source( eva::queue& queue ) :
        eva::source( queue )
    {
}

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            next().get_header()._id = i;
            commit();
        }
    }
};

struct test_processor : public eva::source
{
    test_processor( eva::queue& inp0, eva::queue& inp1, eva::queue& inp2, eva::queue& out0 ) :
         eva::source( out0 ),
        _inp0( inp0.subscriber() ),
        _inp1( inp1.subscriber() ),
        _inp2( inp2.subscriber() )
    {
    }

    virtual void run()
    {
        for( int i=0; i<N; i++ )
        {
            // check input events
            const eva::event& e0 = _inp0->next();
            assert( e0.get_header()._id == i );

            const eva::event& e1 = _inp1->next();
            assert( e0.get_header()._id == i );

            const eva::event& e2 = _inp2->next();
            assert( e0.get_header()._id == i );

            // produce output event
            next().get_header()._id = i;
            commit();
        }
    }

    eva::subscriber* _inp0;
    eva::subscriber* _inp1;
    eva::subscriber* _inp2;
};

struct test_sink : public eva::thread
{
    test_sink( eva::queue& queue ) :
        _inp( queue.subscriber() )
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

    eva::subscriber* _inp;
};

int main()
{
    eva::stopwatch watch;
    eva::queue* queue0 = new eva::queue();
    eva::queue* queue1 = new eva::queue();
    eva::queue* queue2 = new eva::queue();
    eva::queue* queue3 = new eva::queue();

    test_source source0( *queue0 );
    test_source source1( *queue1 );
    test_source source2( *queue2 );
    test_processor processor( *queue0, *queue1, *queue2, *queue3 );
    test_sink sink( *queue3 );

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
