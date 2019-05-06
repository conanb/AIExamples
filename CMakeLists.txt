cmake_minimum_required (VERSION 2.6)
project (aiExamples)

set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set (GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set (GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set (GLFW_INSTALL OFF CACHE BOOL "Generate installation target")

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory (thirdparty/glfw)

add_subdirectory (appToolkit)
add_subdirectory (aiToolkit)

add_subdirectory (examples/BasicExample)

set_target_properties (glfw PROPERTIES FOLDER thirdparty/glfw)
set_target_properties (appToolkit PROPERTIES FOLDER libs/appToolkit)
set_target_properties (aiToolkit PROPERTIES FOLDER libs/aiToolkit)
set_target_properties (BasicExample PROPERTIES FOLDER examples)

