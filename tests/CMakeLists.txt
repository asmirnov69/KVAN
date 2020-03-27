project(test-actions.tsk)

include_directories(${top}/include)
link_directories()

add_executable(${PROJECT_NAME} test-actions.cpp)
target_link_libraries(${PROJECT_NAME} ymlconfig yaml)

