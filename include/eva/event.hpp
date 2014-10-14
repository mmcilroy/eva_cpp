#pragma once

#include <ostream>

namespace eva {

struct event_header
{
    int _id;
    int _type;
    int _source;
    int _length;
    bool _recovery;
};

class event
{
public:
    enum{ size = 256 };

    char* get_raw();
    const char* get_raw() const;

    size_t get_length() const;

    event_header& get_header();
    const event_header& get_header() const;

    char* get_body_buf();
    const char* get_body_buf() const;

    template< class body_type > body_type& get_body();
    template< class body_type > const body_type& get_body() const;

private:
    char _buffer[size];
};

#include "event.inl"

}

inline std::ostream& operator<<( std::ostream& o, const eva::event_header& eh )
{
    o << "event_header: id=" << eh._id
      << ", type=" << eh._type
      << ", length=" << eh._length
      << ", source=" << eh._source
      << ", recovery=" << eh._recovery;
    return o;
}
