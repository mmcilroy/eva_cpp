#pragma once

#include <sys/time.h>

namespace eva {

class stopwatch
{
public:
    void start();
    float elapsed_s();
    unsigned int elapsed_ms();

private:
    struct timeval start_time;
};

}
