#pragma once

#include <map>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace eva {

#define LOG_DEBUG( l, m ) { std::stringstream __ss; __ss << std::endl << "DBG " << ::filename( __FILE__ ) << ":" << __LINE__ << " - " << m << std::endl; l.write( ::log::DEBUG, __ss.str() ); }
#define LOG_INFO( l, m )  { std::stringstream __ss; __ss << std::endl << "INF " << ::filename( __FILE__ ) << ":" << __LINE__ << " - " << m << std::endl; l.write( ::log::INFO, __ss.str() );  }
#define LOG_ERROR( l, m ) { std::stringstream __ss; __ss << std::endl << "ERR " << ::filename( __FILE__ ) << ":" << __LINE__ << " - " << m << std::endl; l.write( ::log::ERROR, __ss.str() ); }

void logger_cleanup();

class log
{
public:
    typedef enum {
        DEBUG, INFO, WARN, ERROR
    } level;

    log( level l );
    void set_level( level l );
    virtual void write( level l, const std::string& s );

private:
    level _level;
};

class logger
{
public:
    static logger& instance();
    log& get( const std::string& id );
    ~logger();

private:
    logger();

    typedef std::map< std::string, log* > logmap;
    logmap _logs;
    log _default_log;

    static logger* _instance;
};

#include "log.inl"

}
