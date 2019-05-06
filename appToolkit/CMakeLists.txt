
include_directories ("${PROJECT_SOURCE_DIR}/appToolkit" 
                     "${PROJECT_SOURCE_DIR}/thirdparty" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glfw/include" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glm"
                     "${PROJECT_SOURCE_DIR}/thirdparty/imgui"
                     "${PROJECT_SOURCE_DIR}/thirdparty/stb")

file(GLOB LIB_SOURCE "*.h" "*.c" "*.cpp")

add_library(appToolkit ${LIB_SOURCE})
target_link_libraries(appToolkit glfw)