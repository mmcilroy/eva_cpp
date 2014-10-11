#include "eva/queue.hpp"

int main()
{
    eva::queue queue;
    while( 1 ) {
        const eva::event& e = queue.subscriber()->next();
    }
}
