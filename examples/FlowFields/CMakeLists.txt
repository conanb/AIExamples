
include_directories ("${PROJECT_SOURCE_DIR}/appToolkit" 
                     "${PROJECT_SOURCE_DIR}/aiToolkit" 
                     "${PROJECT_SOURCE_DIR}/thirdparty" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glfw/include" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glm"
                     "${PROJECT_SOURCE_DIR}/thirdparty/imgui"
                     "${PROJECT_SOURCE_DIR}/thirdparty/stb")

file(GLOB SRC "*.h" "*.cpp" "*.c")

add_executable(FlowFields ${SRC})
target_link_libraries(FlowFields aiToolkit appToolkit)