#include "eva/source.hpp"
#include "eva/stopwatch.hpp"

const int N = 10*1024*1024;

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
    test_source source0( *queue0 );
    test_sink sink0( *queue0 );

    watch.start();
    sink0.start();
    source0.start();
    sink0.join();
    source0.join();

    std::cout << N << " events in " << watch.elapsed_ms() << " ms. " << (int)( N / watch.elapsed_s() ) << " per sec" << std::endl;
}
