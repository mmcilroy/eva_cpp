add_library( eva SHARED event_application.cpp event_node.cpp event_queue.cpp )
target_link_libraries( eva )
install( TARGETS eva DESTINATION lib )

add_subdirectory( test )
