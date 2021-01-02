project(kvan)

include_directories(${top}/cpp/include)
file(GLOB SRC_FILES *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} yajl yaml stdc++fs)
