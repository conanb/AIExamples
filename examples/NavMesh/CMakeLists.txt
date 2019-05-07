
include_directories ("${PROJECT_SOURCE_DIR}/appToolkit" 
                     "${PROJECT_SOURCE_DIR}/aiToolkit" 
                     "${PROJECT_SOURCE_DIR}/thirdparty" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glfw/include" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glm"
                     "${PROJECT_SOURCE_DIR}/thirdparty/imgui"
                     "${PROJECT_SOURCE_DIR}/thirdparty/stb")

file(GLOB SRC "*.h" "*.cpp" "*.c"
			  "poly2tri/poly2tri.h" 
			  "poly2tri/common/*.h" 
			  "poly2tri/common/*.cc" 
			  "poly2tri/sweep/*.h" 
			  "poly2tri/sweep/*.cc" )

add_executable(NavMesh ${SRC})
target_link_libraries(NavMesh aiToolkit appToolkit)