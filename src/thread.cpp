#include "eva/thread.hpp"

using namespace eva;

thread::thread()
{
}

void thread::start()
{
    _run = true;
    _thread = boost::shared_ptr< boost::thread >(
        new boost::thread( boost::bind( &thread::run, this ) ) );
}

void thread::stop()
{
    _run = false;
}

void thread::join()
{
    _thread->join();
}

void thread::run()
{
    while( _run ) {
        loop();
    }
}

void thread::loop()
{
}
