#include "eva/event_journal.hpp"
#include "eva/eva.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "event_replicate" );
}

event_journal::event_journal( const char* filename, bool truncate ) :
    _filename( filename )
{
    // create file if it does not exist
    std::ofstream( filename, std::ofstream::binary | ( truncate ? std::ofstream::trunc : std::ofstream::app ) );

    // open file for binary rw operations
    _file.open( filename, std::fstream::in | std::fstream::out | std::fstream::binary );
    if( !_file.is_open() || !_file.good() ) {
        eva_exception( "failed to open journal" );
    }
}

void event_journal::write( const event& e )
{
    LOG_DEBUG( l, "journal " << e.get_length() << " bytes" );
    _file.write( (char*)&e.get_header(), e.get_length() );
    if( !_file.good() ) {
        eva_exception( "failed to write to journal" );
    }
}

void event_journal::flush()
{
    _file.flush();
    if( !_file.good() ) {
        eva_exception( "failed to flush journal" );
    }
}

void event_journal::recover( event_publisher& p )
{
    event_journal file( _filename.c_str(), false );

    while( file._file.good() )
    {
        event &e = p.next();
        if( !file.read_bytes( (char*)&e.get_header(), sizeof( event_header ) ) ||
            !file.read_bytes( e.get_body_buf(), e.get_header()._length ) ) {
            break;
        }
        e.get_header()._recovery = true;
        p.commit();
    }
}

bool event_journal::read_bytes( char* buf, size_t len )
{
    _file.read( buf, len );
    return ( _file.good() && _file.gcount() == len );
}

event_journal_thread::event_journal_thread( event_subscriber& s ) :
    event_node( "journal" ),
    _subscriber( &s ),
    _journal( "journal.events", false )
{
}

void event_journal_thread::recover( event_publisher& pub )
{
    _journal.recover( pub );
}

void event_journal_thread::run()
{
    while( 1 )
    {
        const event& e = _subscriber->next();
        if( !e.get_header()._recovery )
        {
            LOG_INFO( l, "journal: " << e.get_header() );
            _journal.write( e );
            _journal.flush();
        }
    }
}
