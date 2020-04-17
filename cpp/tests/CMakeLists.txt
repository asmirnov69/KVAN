project(test-actions.tsk)

include_directories(${top}/cpp/include)
link_directories(${top}/cpp/src /usr/local/misc/lib)

add_executable(test-actions.tsk test-actions.cpp)
target_link_libraries(test-actions.tsk KVAN mcpp yaml)

add_executable(test-ymlconfig.tsk test-ymlconfig.cpp)
target_link_libraries(test-ymlconfig.tsk KVAN mcpp yaml)

