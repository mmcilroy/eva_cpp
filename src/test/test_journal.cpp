#include "test_event.hpp"
#include "eva/journal.hpp"
#include "eva/log.hpp"

eva::queue q;
eva::publisher* p = q.publisher();
eva::subscriber* s = q.subscriber();

void write( eva::journal& j, int i )
{
    eva::event e;
    write_event_payload( e, i );
    j.write( e );
    j.flush();
}

int main()
{
    eva::logger::instance().get( "all" ).set_level( eva::log::DEBUG );

    {
        eva::journal j( "test.event", true );
        write( j, 111111 );
        write( j, 222222 );
        write( j, 333333 );
    }

    {
        eva::journal j( "test.event", false );
        j.recover( *p );
        assert( s->next().get_header()._id == 111111 );
        assert( s->next().get_header()._id == 222222 );
        assert( s->next().get_header()._id == 333333 );
        write( j, 444444 );
        write( j, 555555 );
        write( j, 666666 );

    }

    {
        eva::journal j( "test.event", false );
        j.recover( *p );
        assert( s->next().get_header()._id == 111111 );
        assert( s->next().get_header()._id == 222222 );
        assert( s->next().get_header()._id == 333333 );
        assert( s->next().get_header()._id == 444444 );
        assert( s->next().get_header()._id == 555555 );
        assert( s->next().get_header()._id == 666666 );
    }
}
