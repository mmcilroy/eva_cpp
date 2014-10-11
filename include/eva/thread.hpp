#pragma once

#include <boost/thread.hpp>

namespace eva {

class thread
{
public:
    thread();

    void start();
    void stop();
    void join();

protected:
    virtual void run();
    virtual void loop();

    bool _run;
    boost::shared_ptr< boost::thread > _thread;
};

}
