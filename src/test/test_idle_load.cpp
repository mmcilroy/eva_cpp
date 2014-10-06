#include "eva/event_queue.hpp"

int main()
{
    eva::event_queue queue;
    while( 1 ) {
        const eva::event& e = queue.subscriber()->next();
    }
}
