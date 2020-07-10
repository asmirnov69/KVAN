project(vector-io.tsk)

include_directories(${top}/cpp/include)
link_directories(/usr/local/philotymic/3rdparty/lib)


add_executable(${PROJECT_NAME} vector-io.cpp)
target_link_libraries(${PROJECT_NAME} KVAN mcpp yaml)
