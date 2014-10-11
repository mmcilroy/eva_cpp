add_library( eva SHARED journal.cpp log.cpp queue.cpp replicate.cpp source.cpp stopwatch.cpp tcp.cpp tcp_acceptor.cpp thread.cpp )
target_link_libraries( eva )
install( TARGETS eva DESTINATION lib )
add_subdirectory( test )
