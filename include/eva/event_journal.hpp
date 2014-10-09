#pragma once

#include "eva/event_node.hpp"
#include "eva/event_queue.hpp"
#include <fstream>

namespace eva {

class event_journal
{
public:
    event_journal( const char* filename, bool truncate );

    void recover( event_publisher& );
    void write( const event& );
    void flush();

private:
    bool read_bytes( char* buf, size_t len );
    std::fstream _file;
    std::string _filename;
};

class event_journal_thread : public event_node
{
public:
    event_journal_thread( event_subscriber& );
    void recover( event_publisher& );

protected:
    virtual void run();

private:
    event_subscriber* _subscriber;
    event_journal _journal;
};

}
