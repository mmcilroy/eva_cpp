#include "eva/source.hpp"

using namespace eva;

source::source( queue& queue ) :
    _queue( &queue ),
    _out( queue.publisher() )
{
}

eva::publisher& source::publisher()
{
    return *_out;
}

event& source::next()
{
    return _out->next();
}

void source::commit()
{
    _out->commit();
}

void source::run()
{
    _queue->recover();
    while( _run ) {
        loop();
    }
}

void source::loop()
{
}
