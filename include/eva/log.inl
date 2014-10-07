inline const char* filename( const char* s )
{
    return strrchr( s, '/' ) ? strrchr( s, '/' ) + 1 : s;
}

inline log::log( level l )
{
    _level = l;
}

inline void log::set_level( level l )
{
    _level = l;
}

inline void log::write( level l, const std::string& s )
{
    if( l >= _level ) std::cout << s;
}


inline log& logger::get( const std::string& id )
{
    logmap::const_iterator it = _logs.find( id );
    if( it == _logs.end() ) {
        return _default_log;
    } else {
        return *it->second;
    }
}

inline logger::logger()
: _default_log( log::INFO )
{
    atexit( logger_cleanup );
}

inline logger::~logger()
{
    logmap::const_iterator it = _logs.begin();
    for( ; it != _logs.begin(); it++ ) {
        delete it->second;
    }
}
