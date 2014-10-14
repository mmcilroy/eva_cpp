#include "eva/journal.hpp"
#include "eva/eva.hpp"
#include "eva/log.hpp"

using namespace eva;

namespace {
    eva::log& l = eva::logger::instance().get( "journal" );
}

journal::journal( const char* filename, bool truncate ) :
    _filename( filename )
{
    // create file if it does not exist
    if( truncate ) {
        std::ofstream( filename, std::ofstream::binary | std::ofstream::trunc );
    } else {
        std::ofstream( filename, std::ofstream::binary | std::ofstream::app );
    }

    // open file for binary rw operations
    _file.open( filename, std::fstream::in | std::fstream::out | std::fstream::binary );
    if( !_file.is_open() || !_file.good() ) {
        eva_exception( "failed to open journal" );
    }
}

void journal::write( const event& e )
{
    LOG_DEBUG( l, "journal " << e.get_length() << " bytes" );
    _file.seekp( 0, std::ios_base::end );
    _file.write( (char*)&e.get_header(), e.get_length() );
    if( !_file.good() ) {
        eva_exception( "failed to write to journal" );
    }
}

void journal::flush()
{
    _file.flush();
    if( !_file.good() ) {
        eva_exception( "failed to flush journal" );
    }
}

void journal::recover( publisher& p )
{
    LOG_DEBUG( l, "recovering from " << _filename );

    journal file( _filename.c_str(), false );
    file._file.seekg( 0 );
    while( file._file.good() )
    {
        event &e = p.next();
        if( !file.read_bytes( (char*)&e.get_header(), sizeof( event_header ) ) ||
            !file.read_bytes( e.get_body_buf(), e.get_header()._length ) ) {
            break;
        }
        e.get_header()._recovery = true;
        LOG_DEBUG( l, "recovered " << e.get_header() );
        p.commit();
    }
}

bool journal::read_bytes( char* buf, size_t len )
{
    _file.read( buf, len );
    return ( _file.good() && _file.gcount() == len );
}

journal_thread::journal_thread( subscriber& s ) :
    _subscriber( &s ),
    _journal( "journal.events", false )
{
}

void journal_thread::recover( publisher& pub )
{
    _journal.recover( pub );
}

void journal_thread::loop()
{
    const event& e = _subscriber->next();
    if( !e.get_header()._recovery )
    {
        LOG_INFO( l, "journal: " << e.get_header() );
        _journal.write( e );
        _journal.flush();
    }
}
