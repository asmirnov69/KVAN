project(fuargs)

include_directories(${top}/include ${BNFLITE_HOME}/include)
file(GLOB SRC_FILES *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
