#include "eva/event_io.hpp"
#include "eva/log.hpp"
#include "test_event.hpp"

eva::log& l = eva::logger::instance().get( "test_event_server" );

eva::event e;
int i=200000;

struct server_callback : public eva::event_io_callback
{
    void on_event( eva::event_io_session& sess, const eva::event& ei )
    {
        LOG_DEBUG( l, "< " << ei );
        delete &ei;

        write_event_payload( e, i++ );
        sess.write( e );
        sess.read();
    }

    eva::event& alloc_event()
    {
        return *new eva::event;
    }
};

int main()
{
    l.set_level( eva::log::DEBUG );
    eva::event_io_controller ioc;
    ioc.accept( 14002, eva::event_io_callback_ptr( new server_callback ) );
    ioc.run();
}
