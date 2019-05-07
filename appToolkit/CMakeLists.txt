
include_directories ("${PROJECT_SOURCE_DIR}/appToolkit" 
                     "${PROJECT_SOURCE_DIR}/thirdparty" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glfw/include" 
                     "${PROJECT_SOURCE_DIR}/thirdparty/glm"
                     "${PROJECT_SOURCE_DIR}/thirdparty/imgui"
                     "${PROJECT_SOURCE_DIR}/thirdparty/stb")

file(GLOB LIB_SOURCE "*.h" "*.c" "*.cpp" 
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/examples/imgui_impl_glfw.h" 
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/examples/imgui_impl_glfw.cpp"
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/examples/imgui_impl_opengl3.h" 
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/examples/imgui_impl_opengl3.cpp"
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp"
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp"
					 "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp")

add_library(appToolkit ${LIB_SOURCE})
target_link_libraries(appToolkit glfw opengl32.lib)
target_compile_definitions(appToolkit PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM="gl_core_4_4.h")