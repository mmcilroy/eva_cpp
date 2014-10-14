inline char* event::get_raw()
{
    return _buffer;
}

inline const char* event::get_raw() const
{
    return _buffer;
}

inline size_t event::get_length() const
{
    return sizeof( event_header ) + get_header()._length;
}

inline event_header& event::get_header()
{
    return *(event_header*)( _buffer );
}

inline const event_header& event::get_header() const
{
    return *(event_header*)( _buffer );
}

inline char* event::get_body_buf()
{
    return _buffer + sizeof( event_header );
};

inline const char* event::get_body_buf() const
{
    return _buffer + sizeof( event_header );
};

template< class body_type >
inline body_type& event::get_body()
{
    return *(body_type*)( _buffer + sizeof( event_header ) );
};

template< class body_type >
inline const body_type& event::get_body() const
{
    return *(body_type*)( _buffer + sizeof( event_header ) );
};
