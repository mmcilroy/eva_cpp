#include "test_event.hpp"
#include "eva/tcp.hpp"
#include "eva/log.hpp"

eva::log& l = eva::logger::instance().get( "test_event_server" );

eva::event e;
int i=200000;

struct server_callback : public eva::tcp_callback
{
    void on_event( eva::tcp_session& sess, const eva::event& ei )
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
    eva::tcp_controller tcp;
    tcp.accept( 14002, eva::tcp_callback_ptr( new server_callback ) );
    tcp.run();
}
