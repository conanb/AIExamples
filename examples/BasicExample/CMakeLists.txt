cmake_minimum_required (VERSION 2.6)
project (aiExamples)

include_directories ("${PROJECT_SOURCE_DIR}/../appToolkit" 
                     "${PROJECT_SOURCE_DIR}/../aiToolkit" 
                     "${PROJECT_SOURCE_DIR}/../thirdparty" 
                     "${PROJECT_SOURCE_DIR}/../thirdparty/glfw/include" 
                     "${PROJECT_SOURCE_DIR}/../thirdparty/glm"
                     "${PROJECT_SOURCE_DIR}/../thirdparty/imgui"
                     "${PROJECT_SOURCE_DIR}/../thirdparty/stb")

file(GLOB SRC "*.h" "*.cpp" "*.c")

add_executable(basicExample ${SRC})
target_link_libraries(basicExample aiToolkit appToolkit)