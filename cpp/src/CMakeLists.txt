project(kvan)

include_directories(${top}/cpp/include ${YAJL_HOME}/include)
file(GLOB SRC_FILES *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
