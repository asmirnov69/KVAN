project(csv-io.tsk)

include_directories(${top}/cpp/include)
link_directories(${YAJL_HOME}/lib)

add_executable(${PROJECT_NAME} csv-io.cpp)
target_link_libraries(${PROJECT_NAME} KVAN yaml yajl stdc++fs)
