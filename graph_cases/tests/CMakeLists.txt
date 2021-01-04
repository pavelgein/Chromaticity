SET(SOURCE_FILES
    main.cpp
    test_writer.cpp
    test_executer.cpp
    test_sigma.cpp
    test_combinatorics.cpp
    test_multipartite_graphs.cpp
    test_queue.cpp
    test_subsets.cpp
    test_autoindexer.cpp
    test_optparser.cpp
    test_singleton.cpp
    test_binomial_coefficients.cpp
    test_factorial.cpp
)

SET(LIBRARIES
    binomial_coefficients
    test_system
    executer
    multithread_writer
    math_utils
    multipartite_graphs
    optparser
    factorial
)

ADD_EXECUTABLE(tests ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(tests ${LIBRARIES})

