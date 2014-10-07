#include "eva/event_io.hpp"
#include "eva/log.hpp"
#include "test_event.hpp"

eva::log& l = eva::logger::instance().get( "test_event_client" );

eva::event e;
int i=100000;

struct client_callback : public eva::event_io_callback
{
    void on_open( eva::event_io_session& sess )
    {
        write_event_payload( e, i++ );
        sess.write( e );
        sess.read();
    }

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
    ioc.connect( "0.0.0.0", 14002, eva::event_io_callback_ptr( new client_callback ) );
    ioc.run();
}
