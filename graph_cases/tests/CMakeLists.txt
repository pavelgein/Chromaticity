add_executable(tests main.cpp test_writer.cpp test_executer.cpp)
target_link_libraries(tests lib test_system executer multithread_writer)

