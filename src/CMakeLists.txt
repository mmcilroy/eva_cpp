add_library( eva SHARED event_application.cpp event_node.cpp event_queue.cpp event_journal.cpp event_replicate.cpp stopwatch.cpp log.cpp )
target_link_libraries( eva )
install( TARGETS eva DESTINATION lib )

add_subdirectory( test )
