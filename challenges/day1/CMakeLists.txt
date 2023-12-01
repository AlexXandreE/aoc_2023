project(day1)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp")

add_executable(day1 ${SOURCES})


