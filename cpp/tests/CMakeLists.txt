project(test-actions.tsk)

include_directories(${top}/cpp/include)
link_directories(${YAJL_HOME}/lib)

add_executable(test-topdir.tsk test-topdir.cpp)
target_link_libraries(test-topdir.tsk KVAN yaml yajl stdc++fs)

add_executable(test-actions.tsk test-actions.cpp)
target_link_libraries(test-actions.tsk KVAN yaml yajl stdc++fs)

add_executable(test-ymlconfig.tsk test-ymlconfig.cpp)
target_link_libraries(test-ymlconfig.tsk KVAN yaml yajl stdc++fs)

