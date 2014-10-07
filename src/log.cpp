#include "eva/log.hpp"

using namespace eva;

logger* logger::_instance = 0;

namespace eva {
void logger_cleanup()
{
    delete &::logger::instance();
}
}

logger& logger::instance()
{
    if( !_instance ) {
        _instance = new logger;
    }
    return *_instance;
}
