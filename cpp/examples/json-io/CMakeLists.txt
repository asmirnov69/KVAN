project(json-io.tsk)

include_directories(${top}/cpp/include)

add_executable(${PROJECT_NAME} json-io.cpp)
target_link_libraries(${PROJECT_NAME} kvan)
