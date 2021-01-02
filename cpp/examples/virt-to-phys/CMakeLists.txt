project(virt-to-phys.tsk)

include_directories(${top}/cpp/include)

add_executable(${PROJECT_NAME} t.cpp)
target_link_libraries(${PROJECT_NAME} kvan)
