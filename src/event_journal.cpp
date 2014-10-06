#include "eva/event_journal.hpp"
#include "eva/eva.hpp"

using namespace eva;

event_journal::event_journal( const char* filename, bool truncate )
{
    // create file if it does not exist
    std::ofstream( filename, std::ofstream::binary | ( truncate ? std::ofstream::trunc : std::ofstream::app ) );

    // open file for binary rw operations
    _file.open( filename, std::fstream::in | std::fstream::out | std::fstream::binary );
    if( !_file.is_open() ) {
        eva_exception( "failed to open journal" );
    }
}

void event_journal::write( const event& e )
{
    _file.write( (char*)&e.get_header(), e.get_length() );
}

void event_journal::flush()
{
    _file.flush();
}

void event_journal::recover( event_publisher& p )
{
    while( _file.good() )
    {
        event &e = p.next();
        if( !read_bytes( (char*)&e.get_header(), sizeof( event_header ) ) ||
            !read_bytes( e.get_body_buf(), e.get_header()._length ) ) {
            break;
        }
        p.commit();
    }
}

bool event_journal::read_bytes( char* buf, size_t len )
{
    _file.read( buf, len );
    return ( _file.good() && _file.gcount() == len );
}

event_journal_thread::event_journal_thread( event_subscriber& s ) :
    _subscriber( &s ),
    _journal( "journal.events", false )
{
}

void event_journal_thread::run()
{
    while( 1 )
    {
        const event& e = _subscriber->next();
        _journal.write( e );
        _journal.flush();
    }
}
