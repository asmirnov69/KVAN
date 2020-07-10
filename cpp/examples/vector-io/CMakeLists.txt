project(vector-io.tsk)

include_directories(${top}/cpp/include)

add_executable(${PROJECT_NAME} vector-io.cpp)
target_link_libraries(${PROJECT_NAME} KVAN yaml stdc++fs)
