ADD_LIBRARY(executer STATIC executer.cpp multithread_executer.cpp multithread_executer.h singlethread_exectuer.cpp singlethread_exectuer.h)

TARGET_LINK_LIBRARIES(executer PUBLIC queue)