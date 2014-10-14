add_executable( test_application1 test_application1.cpp )
target_link_libraries( test_application1 eva pthread boost_system boost_thread )

add_executable( test_application2 test_application2.cpp )
target_link_libraries( test_application2 eva pthread boost_system boost_thread )

include_directories( include "/usr/include/python2.7/" )
link_directories("/usr/lib/python2.7/config-x86_64-linux-gnu/")
add_executable( test_application3 test_application3.cpp )
target_link_libraries( test_application3 eva pthread ${Boost_LIBRARIES} python2.7 )

add_executable( test_event_server test_event_server.cpp )
target_link_libraries( test_event_server eva pthread boost_system boost_thread )

add_executable( test_event_client test_event_client.cpp )
target_link_libraries( test_event_client eva pthread boost_system boost_thread )

add_executable( test_journal test_journal.cpp )
target_link_libraries( test_journal eva pthread boost_system boost_thread )

add_executable( test_event_queue1 test_event_queue1.cpp )
target_link_libraries( test_event_queue1 eva pthread boost_system boost_thread )

add_executable( test_idle_load test_idle_load.cpp )
target_link_libraries( test_idle_load eva pthread boost_system boost_thread )
