SET(SOURCE_FILES
    main.cpp
    test_writer.cpp
    test_executer.cpp
    test_sigma.cpp
)

SET(LIBRARIES
    test_system
    executer
    multithread_writer
    math_utils
    multipartite_graphs
)

ADD_EXECUTABLE(tests ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(tests ${LIBRARIES})

