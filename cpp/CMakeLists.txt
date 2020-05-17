project(KVAN)
exec_program(git ARGS "rev-parse --show-toplevel" OUTPUT_VARIABLE top)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -Wall")

set(BNFLITE_HOME /usr/local/philotymic/3rdparty)

add_subdirectory(src)
add_subdirectory(tests)

