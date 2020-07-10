project(test-csvreader.tsk)

include_directories(${top}/cpp/include)
link_directories()

add_executable(${PROJECT_NAME} test-csvreader.cpp)
#target_link_libraries(${PROJECT_NAME} eventsource datatypes polygon yajl-utils fuargs misc yajl z stdc++fs)
