SET(SOURCE_FILES
    multipartite_graphs.cpp
    graph.cpp
)

SET(LIBRARIES
    autoindexer
)

ADD_LIBRARY(multipartite_graphs STATIC ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(multipartite_graphs PUBLIC ${LIBRARIES})