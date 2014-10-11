#pragma once

#include "eva/thread.hpp"
#include "eva/queue.hpp"
#include <fstream>

namespace eva {

class journal
{
public:
    journal( const char* filename, bool truncate );

    void recover( publisher& );
    void write( const event& );
    void flush();

private:
    bool read_bytes( char* buf, size_t len );

    std::fstream _file;
    std::string _filename;
};

class journal_thread : public thread
{
public:
    journal_thread( subscriber& );

    void recover( publisher& );

protected:
    virtual void loop();

private:
    journal _journal;
    subscriber* _subscriber;
};

}
