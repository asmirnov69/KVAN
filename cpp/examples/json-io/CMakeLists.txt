project(json-io.tsk)

include_directories(${top}/cpp/include)
link_directories(${YAJL_HOME}/lib)

add_executable(${PROJECT_NAME} json-io.cpp)
target_link_libraries(${PROJECT_NAME} kvan yaml yajl stdc++fs)
