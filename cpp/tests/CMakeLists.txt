project(test-actions.tsk)

include_directories(${top}/cpp/include)

add_executable(test-actions.tsk test-actions.cpp)
target_link_libraries(test-actions.tsk KVAN yaml stdc++fs)

add_executable(test-ymlconfig.tsk test-ymlconfig.cpp)
target_link_libraries(test-ymlconfig.tsk KVAN yaml stdc++fs)

