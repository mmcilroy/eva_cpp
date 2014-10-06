inline void wait()
{
    //usleep( 20000 ); 
    pthread_yield();
}

inline sequence::sequence() :
    _val( 0 )
{
}

inline sequence::value sequence::add( size_t s )
{
    __sync_fetch_and_add( &_val, s );
}

inline sequence::value sequence::get() const
{
    return _val;
}

template< class T >
inline ring_buffer< T >::ring_buffer( size_t s ) :
    _buffer( new T[s] ),
    _size( s )
{
}

template< class T >
inline ring_buffer< T >::~ring_buffer()
{
    if( _buffer ) {
        delete [] _buffer;
    }
}

template< class T >
inline T& ring_buffer< T >::next()
{
    return at( _wseq );
}

template< class T >
inline void ring_buffer< T >::commit()
{
    _wseq.add();
}

template< class T >
inline size_t ring_buffer< T >::size() const
{
    return _size;
}

template< class T >
inline T& ring_buffer< T >::at( const sequence& s ) const
{
    return _buffer[ s.get() % _size ];
}

template< class T >
inline sequence& ring_buffer< T >::get_sequence()
{
    return _wseq;
}

template< class T >
inline reader< T >::reader( ring_buffer< T >& rb ) :
    _rb( rb ),
    _bseq( rb.get_sequence() )
{
}

template< class T >
inline reader< T >::reader( reader< T >& r ) :
    _rb( r._rb ),
    _bseq( r._rseq )
{
}

template< class T >
inline const T& reader< T >::next()
{
    if( available() <= 0 )
    {
        while( available() <= 0 ) {
            wait();
        }
    }

    const T& e = _rb.at( _rseq );
    _rseq.add();

    return e;
}

template< class T >
inline size_t reader< T >::available()
{
    return _bseq.get() - _rseq.get();
}

template< class T >
inline writer< T >::writer( reader< T >& r ) :
    _rb( r._rb ),
    _bseq( r._rseq )
{
}

template< class T >
inline T& writer< T >::next()
{
    while( _rb.get_sequence().get() - _bseq.get() >= _rb.size()-1 ) {
        wait();
    }

    return _rb.next();
}

template< class T >
inline void writer< T >::commit()
{
    _rb.commit();
}
