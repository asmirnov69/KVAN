project(logger-test.tsk)

include_directories(${top}/cpp/include)
link_directories(${YAJL_HOME}/lib)

add_executable(${PROJECT_NAME} logger-test.cpp)
target_link_libraries(${PROJECT_NAME} kvan yaml yajl stdc++fs)
