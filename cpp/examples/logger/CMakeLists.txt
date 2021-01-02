project(logger-test.tsk)

include_directories(${top}/cpp/include)

add_executable(${PROJECT_NAME} logger-test.cpp)
target_link_libraries(${PROJECT_NAME} kvan)
