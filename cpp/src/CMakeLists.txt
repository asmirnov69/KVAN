project(KVAN)

include_directories(${top}/cpp/include)
file(GLOB SRC_FILES *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
