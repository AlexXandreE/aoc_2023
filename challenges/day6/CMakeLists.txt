project(day6)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp")

add_executable(day6 ${SOURCES})
