add_executable( test_event_queue1 test_event_queue1.cpp )
target_link_libraries( test_event_queue1 eva pthread boost_system boost_thread )

add_executable( test_event_queue2 test_event_queue2.cpp )
target_link_libraries( test_event_queue2 eva pthread boost_system boost_thread )

add_executable( test_idle_load test_idle_load.cpp )
target_link_libraries( test_idle_load eva pthread boost_system boost_thread )

add_executable( test_app test_app.cpp )
target_link_libraries( test_app eva pthread boost_system boost_thread )
