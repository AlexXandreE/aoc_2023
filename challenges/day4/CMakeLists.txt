project(day4)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp")

add_executable(day4 ${SOURCES})
