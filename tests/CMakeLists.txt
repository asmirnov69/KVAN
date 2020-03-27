project(test-actions.tsk)

include_directories(${top}/include)
link_directories(${top}/src)

add_executable(${PROJECT_NAME} test-actions.cpp)
target_link_libraries(${PROJECT_NAME} fuargs)

