#pragma once

#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

namespace eva {

class sequence
{
public:
    typedef unsigned long long value;

    inline sequence();
    inline value add( size_t s=1 );
    inline value get() const;

private:
    value _val;
};

template< class T >
class ring_buffer
{
public:
    ring_buffer( size_t s );
    ~ring_buffer();

    inline T& next();
    inline T& at( const sequence& s ) const;
    inline void commit();
    inline size_t size() const;
    inline sequence& get_sequence();

private:
    T* _buffer;
    sequence _wseq;
    size_t _size;
};

template< class T >
class reader
{
public:
    reader( ring_buffer< T >& rb );
    reader( reader< T >& r );

    inline const T& next();
    inline size_t available();

//private:
    ring_buffer< T >& _rb;
    sequence& _bseq;
    sequence  _rseq;
};

template< class T >
class writer
{
public:
    writer( reader< T >& r );

    inline T& next();
    inline void commit();

private:
    ring_buffer< T >& _rb;
    sequence& _bseq;
};

#include "disruptor.inl"

}
