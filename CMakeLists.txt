cmake_minimum_required (VERSION 2.6)
project (aiExamples)

set (GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set (GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set (GLFW_INSTALL OFF CACHE BOOL "Generate installation target")

add_subdirectory (thirdparty/glfw)

add_subdirectory (appToolkit)
add_subdirectory (aiToolkit)

add_subdirectory (examples/BasicExample)