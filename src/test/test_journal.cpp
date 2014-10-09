#include "eva/event_journal.hpp"
#include "eva/log.hpp"
#include "test_event.hpp"

eva::event_queue q;
eva::event_publisher* p = q.publisher();
eva::event_subscriber* s = q.subscriber();

void write( eva::event_journal& j, int i )
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
        eva::event_journal j( "test.event", true );
        write( j, 111111 );
        write( j, 222222 );
        write( j, 333333 );
    }

    {
        eva::event_journal j( "test.event", false );
        j.recover( *p );
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        //assert( s->next().get_header()._id == 111111 );
        //assert( s->next().get_header()._id == 222222 );
        //assert( s->next().get_header()._id == 222222 );
        write( j, 444444 );
        write( j, 555555 );
        write( j, 666666 );
    }

    {
        eva::event_journal j( "test.event", false );
        j.recover( *p );
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        std::cout << s->next().get_header()._id << std::endl;
        /*
        assert( s->next().get_header()._id == 111111 );
        assert( s->next().get_header()._id == 222222 );
        assert( s->next().get_header()._id == 222222 );
        assert( s->next().get_header()._id == 444444 );
        assert( s->next().get_header()._id == 555555 );
        assert( s->next().get_header()._id == 666666 );
        */
    }
}
